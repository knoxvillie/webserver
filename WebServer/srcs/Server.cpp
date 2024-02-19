/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 16:30:41 by kfaustin          #+#    #+#             */
/*   Updated: 2024/02/16 19:43:23 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"

//Prototypes:
static std::string defaultServerConfig(int);

Server::Server(void) {}

Server::Server(std::map<std::string, std::vector<std::string> >& server, std::map<std::string,
			   std::map<std::string, std::vector<std::string> > >& location)
			   : _serverDirectives(server), _locationDirectives(location) {
	this->applyServerDirectives();
	this->validateServerDirectives();
}

void
Server::applyServerDirectives(void) {
	std::map<std::string, std::vector<std::string> >::iterator it;

	for (int i = 0; Parser::server_directives[i]; ++i) {
		it = this->_serverDirectives.find(Parser::server_directives[i]);
		if (it == this->_serverDirectives.end()) {
			// Add the key into the map, and initialize it with default values.
			this->_serverDirectives[Parser::server_directives[i]] = splitStringToVector(defaultServerConfig(i));
		}
	}
}

void
Server::validateServerDirectives(void) {
	std::map<std::string, std::vector<std::string> >& server = this->_serverDirectives;
	std::map<std::string, std::map<std::string, std::vector<std::string> > >& location = this->_locationDirectives;
	(void)server; (void) location;
	for (std::map<std::string, std::vector<std::string> >::iterator it = server.begin(); it != server.end(); it++) {
		for (std::vector<std::string>::iterator ut = it->second.begin(); ut != it->second.end(); ut++) {
			// Listen directive
			// Need to create methods to verify each directive later.
			if (it->first == "listen") {
				int port;
				if (it->second.size() > 1)
					throw std::runtime_error("Error: listen directive has too many arguments");
				if ((*ut).find(':') == std::string::npos) {
					this->s_host = "0.0.0.0";
					port = ((*ut).find(';') != std::string::npos) ? std::atoi((*ut).substr(0, (*ut).find(';')).c_str()) : std::atoi((*ut).c_str());
				} else {
					size_t	pos = (*ut).find(';');
					this->s_host = (*ut).substr(0, pos);
					port = (std::atoi((*ut).substr(pos + 1).c_str()));
					if (inet_pton(AF_INET, this->s_host.c_str(), &this->ipAddress) <= 0)
						throw std::runtime_error("IP PROBLEM");
				}
				if (port <= 0 || port > 65535)
					throw std::runtime_error("Error: invalid server port");
				this->s_port = (unsigned short)port;
				break;
			}
		}
	}
}


//Getters
std::map<std::string, std::vector<std::string> >&
Server::getServerDirectives(void) {
	return (_serverDirectives);
}

std::map<std::string, std::map<std::string, std::vector<std::string> > >&
Server::getLocationDirectives(void) {
	return (_locationDirectives);
}

//Static
std::string
defaultServerConfig(int directive) {
	switch (directive) {
		case 0: return ("0.0.0.0:8080"); //Listen
		case 1: return ("default"); //Server_name
		case 2: return ("/home/kfaustin/webserver/"); //Root
		case 3: return ("index.html"); //Index
		case 4: return ("on"); //Autoindex (MAYBE)
		case 5: return ("GET POST DELETE"); //Allow_methods
		case 6: return ("1M"); //Client_max_body_size
		case 7: return ("/error"); //Error_page
		default: throw std::runtime_error("Server.cpp defaultServerConfig Methods");
	}
}