/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 11:35:52 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/26 10:17:29 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Parser.hpp"
#include "TcpServer.hpp"
#include "Config.hpp"

class Cluster {
	
	private:
		std::vector<TcpServer*> m_servers;
		std::vector<int> m_serverSockets;
		std::map<int, TcpServer*> m_fdToServer;
		std::vector<Config> m_configs;

		void deleteServers(void);
		void createServers(void);
		void serversLoop(void);

	public:
		Cluster(const std::vector<Config>& configs);
		~Cluster();
};
