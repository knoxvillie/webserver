/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 16:30:41 by kfaustin          #+#    #+#             */
/*   Updated: 2024/02/29 16:34:45 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Server.hpp"

//Prototypes:
static std::string defaultServerConfig(int);

Server::Server(void) {}

Server::Server(std::map<std::string, std::vector<std::string> >& server, std::map<std::string,
			   std::map<std::string, std::vector<std::string> > >& location)
			   : _serverDirectives(server), _locationDirectives(location) {
	GPS;
	this->applyServerDirectives();
	this->validateServerDirectives();
}

// This method stands to check which "essentials directives" are not in the server block and initialize it.
void
Server::applyServerDirectives(void) {
	std::map<std::string, std::vector<std::string> >::iterator it;

	for (int i = 0; Parser::server_directives[i]; ++i) {
		it = this->_serverDirectives.find(Parser::server_directives[i]);
		// Directive [i] not in the server block
		if (it == this->_serverDirectives.end()) {
			// Add the key into the map, and initialize it with default values.
			// splitStringToVector handles the case that the directive has more than 1 value. (Allow_methods)
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
		if (it->first == "listen")
			this->checkListen(it->second);
		//else if (it->first == "server_name")
	}
}

// Kind of a directives parser
void
Server::checkListen(std::vector<std::string>& vec) {
	GPS;
	if (vec.size() > 1)
		throw std::runtime_error("Error: listen directive has too many arguments");
	int port;
	std::string value(vec[0]);
	if (value.find(':') == std::string::npos) {
		this->s_host = "0.0.0.0";
		port = (value.find(';') != std::string::npos) ? std::atoi(value.substr(0, value.find(';')).c_str()) : std::atoi(value.c_str());
	} else {
		size_t	pos = value.find(';');
		this->s_host = value.substr(0, pos);
		port = (std::atoi(value.substr(pos + 1).c_str()));
		if (inet_pton(AF_INET, this->s_host.c_str(), &this->ipAddress) <= 0)
			throw std::runtime_error("IP PROBLEM");
	}
	if (port <= 0 || port > 65535)
		throw std::runtime_error("Error: invalid server port");
	this->s_port = (uint16_t)port;
}

void
Server::checkServerName(std::vector<std::string>& vec) {
	/*
	 * When you set server_name default; in an Nginx server block, it means that this block will respond
	 * to requests that do not match any other server_name specified in the server configuration.
	 * */
	(void)vec;
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