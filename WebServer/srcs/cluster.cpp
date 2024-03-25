/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 11:10:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/25 12:00:53 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cluster.hpp"

std::vector<TcpServer*> servers;

void createServers(void) {
	std::vector<Server>& server_blocks = Parser::getServers();
	GPS;
	
	for (size_t i = 0; i < server_blocks.size(); i++) {
		TcpServer server(server_blocks[i]);
		servers.push_back(&server);
	}
}

void deleteServers(void) {
	for (size_t i = 0; i < servers.size(); i++) {
		delete servers[i];
		MLOG("done");
	}
}
