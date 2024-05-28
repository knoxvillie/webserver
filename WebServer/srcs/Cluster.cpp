/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 11:10:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/05/28 12:51:42 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"
#include "Http.hpp"
//#include "CgiHandler.hpp"

volatile sig_atomic_t gEndLoop = 0;

std::vector<int> Cluster::serverSockets;
std::map<int, Server*> Cluster::sockToServer;

void
Cluster::startServers(std::vector<Server>& servers) {
	GPS;
	for (size_t i = 0; i < servers.size(); i++) {
		int server_sock = servers[i].getSocket();
		Cluster::serverSockets.push_back(server_sock);
		Cluster::sockToServer[server_sock] = &(servers[i]);
	}
	//CgiHandler::initMap();
	Cluster::serversLoop(servers);
}

/*
	IMPORTANT NOTE: 
	It is normal that after connecting to the server when restarting it takes a while for the
	socket to be able to bind again, this is due to TIME_WAIT, a standard TCP behaviour

	Right now this is not happening because we put SO_REUSEADDR to better test the program
*/

void
Cluster::serversLoop(std::vector<Server>& servers) {
	GPS;
	bool new_connection = true;
	int epoll_fd, num_ready_events;
	struct epoll_event event, event_buffer[MAX_EVENTS];
	std::map<int, Request*> requests;
	std::map<int, Request*> cgi_requests;
	
	event.events = EPOLLIN; // The server sockets only need to be monitored for read calls
	epoll_fd = epoll_create((int)servers.size()); // Expected number of fd, 0 to set to standard
	
	if (epoll_fd < 0)
		throw std::runtime_error("Error: Creating epoll instance");
	for (size_t i = 0; i < servers.size(); i++) {
		event.data.fd = servers[i].getSocket();
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, servers[i].getSocket(), &event) < 0) {
			std::cerr << "Error adding socket to epoll: " << strerror(errno) << std::endl;
			throw std::runtime_error("Error: epoll_ctl_add failed");
		}
	}
	while (!gEndLoop) {
		if (new_connection) {
			std::cout << std::endl << "========================\n";
			std::cout << "Waiting for connections...\n";
			std::cout << "========================\n\n" << std::endl;
		}
		num_ready_events = epoll_wait(epoll_fd, event_buffer, MAX_EVENTS, -1);
		if (num_ready_events < 0)
			std::cout << "EVENT < 0" << std::endl;

		for (int i = 0; i < num_ready_events; i++) {
			int client_sock = event_buffer[i].data.fd;

			// client_sock is in Cluster::serverSockets, so it's a new connection
			if (std::find(Cluster::serverSockets.begin(), Cluster::serverSockets.end(), client_sock) != Cluster::serverSockets.end()) {
				client_sock = Cluster::sockToServer[event_buffer[i].data.fd]->acceptConnection();
				Cluster::sockToServer[client_sock] = Cluster::sockToServer[event_buffer[i].data.fd];
				
				// Set socket as non-blocking
				if (fcntl(client_sock, F_SETFL, O_NONBLOCK) < 0)
					throw std::runtime_error("ERROR - Server: failed to set socket as nonblocking.");
				
				event_buffer[i].data.fd = client_sock;
				event_buffer[i].events = EPOLLIN | EPOLLOUT;// | EPOLLET; // EPOLLET = Edge Triggered mode 
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event_buffer[i]) < 0)
					throw std::runtime_error("Error: epoll_ctl failed");	
				new_connection = false;
			}
			else {
				
				if (event_buffer[i].events & EPOLLERR) {
					//MLOG("EPOLLERR is present\n");
					if (requests.find(client_sock) != requests.end()) { // There is an allocated request for this socket
						delete requests[client_sock];
						requests.erase(client_sock);
					}
					Cluster::closeConnection(epoll_fd, client_sock);
					// TODO: Check if the requests is deleted correcly
					// TODO: Check if we should send some error response to the client
					continue;
				}
				
				if (event_buffer[i].events & EPOLLHUP) {
				    MLOG("EPOLLHUP is present\n");
					if (requests.find(client_sock) != requests.end()) { // There is an allocated request for this socket
						delete requests[client_sock];
						requests.erase(client_sock);
					}
					Cluster::closeConnection(epoll_fd, client_sock);
					// TODO: Check if the requests is deleted correcly
					// TODO: Check if we should send some error response to the client
					continue;
				}
				
				if (event_buffer[i].events & EPOLLIN) { // Not closing these sockets yet
				    MLOG("EPOLLIN is present\n");
					
					if (cgi_requests.find(client_sock) != cgi_requests.end()) { // in this case client_sock is a fd
						// TODO: checks
						if (!cgi_requests[client_sock]->cgi_finished)
							CgiHandler::readFromCgi(client_sock, *cgi_requests[client_sock]);
						else {
							cgi_requests.erase(client_sock);
							close(client_sock);
						}
						continue;
					}
					
					// If there are no previous request for this client_sock create a new one
					if (requests.find(client_sock) == requests.end()) {
						Request *cl_request = new Request(Cluster::sockToServer[client_sock]);
						requests[client_sock] = cl_request;	
					} else if (requests[client_sock]->isFinished()) continue;
					
					Http::receiveFromClient(client_sock, *requests[client_sock]);
				}
				
				if (event_buffer[i].events & EPOLLOUT) {
					Response* response;
					
					if (cgi_requests.find(client_sock) != cgi_requests.end()) { // in this case client_sock is a fd
						// Only the pipe to child will get here, since we only monitor for EPOLLOUT there 
						CgiHandler::writeToCgi(client_sock, *cgi_requests[client_sock]);
						// Write to CGI once
						cgi_requests.erase(client_sock);
						close(client_sock);
						continue;
					}
					
					// EPOLLOUT event means that the socket is ready for writing
					if (requests.find(client_sock) == requests.end()) continue; // No previous request for this client_sock
					
					// If the request isn't finished don't send a response
					if (!(requests[client_sock]->isFinished())) continue;

				    //MLOG("EPOLLOUT is present\n");
					
					// This try catch is just to help in the methods and what not where returning is not as pratical
					//	DO NOT THROW AN EXCEPTION FOR EVERY HTTP ERROR
					try { 
						// If the request is not a CGI
						if (!requests[client_sock]->isCGI()) 
							response = Http::BuildResponse(*requests[client_sock]);
						
						// Can't put else here because it need to enter BuildResponse() the first cycle and also into this if
						if (requests[client_sock]->isCGI()) {
							// The request is a cgi
							if (requests[client_sock]->cgi_finished) {
								// CGI is finished so send the response
								response = new Response(requests[client_sock]->cgiBuf);
							} else if (requests[client_sock] == (find_by_value(cgi_requests, requests[client_sock]))->second)
								// This means we found the request in cgi_requests, meaning we already executed the CGI but its not finished yet
								continue;
							else {
								// This mean that its the first time the CGI request is passing through the loop
								// After executing Http::BuildResponse()
								struct epoll_event pipe_event;
        						// Add the pipe_to_parent[0] to epoll (to read from CGI)
								pipe_event.events = EPOLLIN;
        						pipe_event.data.fd = requests[client_sock]->cgi_pipes[0];
								if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, requests[client_sock]->cgi_pipes[0], &pipe_event) < 0)
									throw std::runtime_error("Error: epoll_ctl failed");
								// Add the pipe_to_child[1] to epoll (to write to CGI)
								pipe_event.events = EPOLLOUT;
								pipe_event.data.fd = requests[client_sock]->cgi_pipes[1];
								if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, requests[client_sock]->cgi_pipes[1], &pipe_event) < 0)
									throw std::runtime_error("Error: epoll_ctl failed");
								
								cgi_requests[requests[client_sock]->cgi_pipes[0]] = requests[client_sock];
								cgi_requests[requests[client_sock]->cgi_pipes[1]] = requests[client_sock];
								// TODO: Might need to do more stuff idk
								continue;
							}
						}


					} catch (const Http::HttpErrorException& e) {
						e.what();
						response = new Response(e.getErrorCode(), Cluster::sockToServer[client_sock]);	
					}
					response->sendToClient(client_sock);
					delete response;
					if (requests[client_sock]->isToClose()) {
						Cluster::closeConnection(epoll_fd, client_sock);
						new_connection = true; // TODO: Check if this is right
					}
					delete requests[client_sock];
					requests.erase(client_sock);
				}
				
				// TODO: Check if this is right
				//Cluster::closeConnection(epoll_fd, client_sock);
			}
		}
	}
	Cluster::deleteRequests(requests);
	Cluster::deleteServers(epoll_fd);
	close(epoll_fd);
}

void
Cluster::deleteRequests(std::map<int, Request*>& requests) {
	std::map<int, Request*>::iterator it;
	for (it = requests.begin(); it != requests.end(); it++) {
		delete (it->second);
	}
}

void
Cluster::closeConnection(int epoll_fd, int client_sock) {
	Cluster::sockToServer.erase(client_sock);
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sock, NULL);
	close(client_sock);
}

void
Cluster::deleteServers(int epoll_fd) {
	GPS;

	for (size_t i = 0; i < Cluster::serverSockets.size(); i++) {
		epoll_ctl(epoll_fd, EPOLL_CTL_DEL, Cluster::serverSockets[i], NULL);
		if (close(Cluster::serverSockets[i]) < 0) {
			MERROR("couldn't close socket.");
		}
	}
	MLOG("deleted all server");
}
