/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 11:10:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/25 13:50:56 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

Cluster::Cluster(const std::vector<Config>& configs) : m_configs(configs){
	createServers();
}

Cluster::~Cluster() {
	deleteServers();
}

void
Cluster::createServers(void) {
	GPS;
	for (size_t i = 0; i < m_configs.size(); i++) {
		TcpServer server(m_configs[i]);
		m_servers.push_back(&server);
		m_sockets.push_back(server.getSocket());
		m_fdToServer[server.getSocket()] = server;
	}
}

void
Cluster::serversLoop() {
	int epoll_fd, num_ready_events, client_sock;
	struct epoll_event event, event_buffer[MAX_EVENTS];
	
	epoll_fd = epoll_create(1); // probably not 1
	
	if (epoll_fd < 0)
		throw std::runtime_error("Error: Creating epoll instance");
	
	for (size_t i = 0; i < m_sockets.size(); i++) {
		event.events = EPOLLIN;
		event.data.fd = m_sockets[i];
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, m_sockets[i], &event) < 0)
			throw std::runtime_error("Error: epoll_ctl failed");
	}
	
	while (true) {
		num_ready_events = epoll_wait(epoll_fd, event_buffer, MAX_EVENTS, -1);
		
		if (num_ready_events < 0) 
			break; // ????
		
		for (int i = 0; i < num_ready_events; i++) {
			client_sock = event_buffer[i].data.fd;
			
			if (std::find(m_sockets.begin(), m_sockets.end(), client_sock) != m_sockets.end()) {
				// client_sock is in m_sockets
				client_sock = m_fdToServer[client_sock].acceptConnection();
				event_buffer[i].events = EPOLLIN;
				event_buffer[i].data.fd = client_sock;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event_buffer[i]) < 0)
					throw std::runtime_error("Error: epoll_ctl failed");
			}
			else {
				m_fdToServer[client_sock].handleConnection(client_sock);
			}
		}
	}
	close (epoll_fd);
}

void
Cluster::deleteServers(void) {
	for (size_t i = 0; i < m_servers.size(); i++) {
		delete m_servers[i];
		MLOG("done");
	}
}

