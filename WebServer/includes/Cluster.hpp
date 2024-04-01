/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 11:35:52 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/30 23:43:47 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

#include "webserv.hpp"
#include "Parser.hpp"
#include "Server.hpp"

#define MAX_EVENTS 42

class Cluster {
	
	private:
		Cluster(void);
		~Cluster();

		static std::vector<int> serverSockets;
		static std::map<int, Server*> sockToServer;
		static void serversLoop(std::vector<Server>&);

	public:
		static void startServers(std::vector<Server>&);
		static void deleteServers(void);

};

#endif //CLUSTER_HPP