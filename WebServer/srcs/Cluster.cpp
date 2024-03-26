/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 11:10:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/26 11:43:10 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

volatile sig_atomic_t gEndLoop = 0;

void
Cluster::startServers(const std::vector<Config>& configs) {
	GPS;
	m_configs = configs;
	for (size_t i = 0; i < m_configs.size(); i++) {
		TcpServer *server = new TcpServer(m_configs[i]);
		m_servers.push_back(server);
		m_serverSockets.push_back(server->getSocket());
		m_fdToServer[server->getSocket()] = server;
	}
	Cluster::serversLoop();
}

/*
	IMPORTANT NOTE: 
	It is normal that after connecting to the server when restarting it takes a while for the
	socket to be able to bind again, this is due to TIME_WAIT, a standard TCP behaviour
*/

void
Cluster::serversLoop() {
	GPS;

	int epoll_fd, num_ready_events, client_sock;
	struct epoll_event event, event_buffer[MAX_EVENTS];
	
	epoll_fd = epoll_create(1); // probably not 1
	
	if (epoll_fd < 0)
		throw std::runtime_error("Error: Creating epoll instance");
	
	for (size_t i = 0; i < m_serverSockets.size(); i++) {
		event.events = EPOLLIN;
		event.data.fd = m_serverSockets[i];
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, m_serverSockets[i], &event) < 0)
			throw std::runtime_error("Error: epoll_ctl_add failed");
	}
	
	while (!gEndLoop) {
		num_ready_events = epoll_wait(epoll_fd, event_buffer, MAX_EVENTS, -1);
		
		if (num_ready_events < 0) 
			;//break; // ????
		
		for (int i = 0; i < num_ready_events; i++) {
			client_sock = event_buffer[i].data.fd;
			
			if (std::find(m_serverSockets.begin(), m_serverSockets.end(), client_sock) != m_serverSockets.end()) {
				// client_sock is in m_serverSockets, so its a new connection
				client_sock = m_fdToServer[event_buffer[i].data.fd]->acceptConnection();
				m_fdToServer[client_sock] = m_fdToServer[event_buffer[i].data.fd];
				event_buffer[i].events = EPOLLIN;
				event_buffer[i].data.fd = client_sock;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event_buffer[i]) < 0)
					throw std::runtime_error("Error: epoll_ctl failed");
			}
			else {
				m_fdToServer[client_sock]->handleConnection(client_sock);
/* 				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_sock, NULL);
				close(client_sock);
				m_fdToServer.erase(client_sock); */
			}
		}
	}
	close (epoll_fd);
	deleteServers();
}

void
Cluster::deleteServers(void) {
	GPS;

	for (size_t i = 0; i < m_serverSockets.size(); i++) {
		if (close(m_serverSockets[i]) < 0) {
			MERROR("couldn't close socket.");
		}
	}

	for (size_t i = 0; i < m_servers.size(); i++) {
		delete m_servers[i];
	}
	MLOG("deleted all server");
}

