/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 11:35:52 by diogmart          #+#    #+#             */
/*   Updated: 2024/05/28 16:45:34 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

#include "webserv.hpp"
#include "Parser.hpp"
#include "Server.hpp"
#include "Request.hpp"

#define MAX_EVENTS 42

class Cluster {
	
	private:
		Cluster(void);
		~Cluster();

		static std::vector<int> serverSockets;
		static std::map<int, Server*> sockToServer;
		static void serversLoop(std::vector<Server>&);
		static void closeConnection(int epoll_fd, int client_sock);
		static void closeCgiConnection(int epoll_fd, int client_sock, std::map<int, Request*>& cgi_requests);
		static void deleteRequests(std::map<int, Request*>& requests);

	public:
		static void startServers(std::vector<Server>&);
		static void deleteServers(int epoll_fd);

};

#endif //CLUSTER_HPP