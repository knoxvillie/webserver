/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 16:30:41 by kfaustin          #+#    #+#             */
/*   Updated: 2024/02/12 16:47:41 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 12:00:44 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/09 12:26:26 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void) {}

Server::Server(std::map<std::string, std::vector<std::string> >& server, std::map<std::string,
			   std::map<std::string, std::vector<std::string> > >& location)
			   : _serverDirectives(server), _locationDirectives(location) {}

std::map<std::string, std::vector<std::string> > &Server::getServerDirectives(void) {
	return (_serverDirectives);
}

std::map<std::string, std::map<std::string, std::vector<std::string> > > &Server::getLocationDirectives(void) {
	return (_locationDirectives);
}