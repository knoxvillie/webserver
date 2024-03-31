/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 11:10:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/31 00:05:38 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"
#include "HttpRequest.hpp"

volatile sig_atomic_t gEndLoop = 0;

std::vector<int> Cluster::serverSockets;
std::map<int, const Server*> Cluster::sockToServer;

void
Cluster::startServers(const std::vector<Server>& servers) {
	GPS;
	//Cluster::servers = servers;
	for (size_t i = 0; i < servers.size(); i++) {
		int server_sock = servers[i].getSocket();
		Cluster::serverSockets.push_back(server_sock);
		Cluster::sockToServer[server_sock] = &(servers[i]);
	}
	Cluster::serversLoop(servers);
}

/*
	IMPORTANT NOTE: 
	It is normal that after connecting to the server when restarting it takes a while for the
	socket to be able to bind again, this is due to TIME_WAIT, a standard TCP behaviour
*/

void
Cluster::serversLoop(const std::vector<Server>& servers) {
	GPS;

	int epoll_fd, num_ready_events, client_sock;
	struct epoll_event event, event_buffer[MAX_EVENTS];

	event.events = EPOLLIN;
	epoll_fd = epoll_create((int)servers.size()); // Expected number of fd, 0 to set to standard
	
	if (epoll_fd < 0)
		throw std::runtime_error("Error: Creating epoll instance");
	for (size_t i = 0; i < servers.size(); i++) {
		event.data.fd = servers[i].getSocket();
		std::cout << "aaaaaaaaaaaaaaaaaaaaaaa" << event.data.fd << std::endl;
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, servers[i].getSocket(), &event) < 0)
			throw std::runtime_error("Error: AAAAAAA epoll_ctl_add failed");
	}
	while (!gEndLoop) {
		num_ready_events = epoll_wait(epoll_fd, event_buffer, MAX_EVENTS, -1);
		
		//if (num_ready_events < 0);//break; // ????
		
		for (int i = 0; i < num_ready_events; i++) {
			client_sock = event_buffer[i].data.fd;
			
			if (std::find(Cluster::serverSockets.begin(), Cluster::serverSockets.end(), client_sock) != Cluster::serverSockets.end()) {
				// client_sock is in Cluster::serverSockets, so it's a new connection
				client_sock = Cluster::sockToServer[event_buffer[i].data.fd]->acceptConnection();
				Cluster::sockToServer[client_sock] = Cluster::sockToServer[event_buffer[i].data.fd];
				event_buffer[i].events = EPOLLIN;
				event_buffer[i].data.fd = client_sock;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event_buffer[i]) < 0)
					throw std::runtime_error("Error: epoll_ctl failed");
			}
			else {
				HttpRequest request(client_sock, Cluster::sockToServer[client_sock]);
				request.sendResponse(client_sock);
//				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sock, NULL);
//				close(client_sock);
//				Cluster::fdToServer.erase(client_sock);
			}
		}
	}
	close (epoll_fd);
	deleteServers();
}

void
Cluster::deleteServers(void) {
	GPS;

	for (size_t i = 0; i < Cluster::serverSockets.size(); i++) {
		if (close(Cluster::serverSockets[i]) < 0) {
			MERROR("couldn't close socket.");
		}
	}
	MLOG("deleted all server");
}

