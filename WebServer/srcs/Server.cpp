/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 16:30:41 by kfaustin          #+#    #+#             */
/*   Updated: 2024/03/19 15:03:14 by kfaustin         ###   ########.fr       */
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
		else if (it->first == "server_name")
			this->checkServerName(it->second);
		else if(it->first == "root")
			this->checkRoot(it->second);
		else if (it->first == "index")
			this->checkIndex(it->second);
		else if (it->first == "auto_index")
			this->checkAutoIndex(it->second);
		else if (it->first == "allow_methods")
			this->checkAllowMethods(it->second);
		else if (it->first == "client_max_body_size")
			this->checkClientMaxBodySize(it->second);
	}
}

// Kind of a directives parser
void
Server::checkListen(std::vector<std::string>& vec) {
	GPS;
	int port;
	struct in_addr ipAddress;

	std::memset(&ipAddress, 0, sizeof(ipAddress));
	if (vec.size() > 1)
		throw std::runtime_error("Error: listen directive has too many arguments");
	std::string value(vec[0]);
	if (value.find(':') == std::string::npos) {
		this->s_host = "0.0.0.0";
		port = std::atoi(value.substr(0, value.find(';')).c_str());
	} else {
		size_t	pos = value.find(':');
		this->s_host = value.substr(0, pos);
		port = (std::atoi(value.substr(pos + 1).c_str()));
		if (inet_pton(AF_INET, this->s_host.c_str(), &ipAddress) <= 0)
			throw std::runtime_error("IP PROBLEM");
	}
	if (port < 1024 || port > 65535)
		throw std::runtime_error("Error: invalid server port");
	this->s_port = (uint16_t)port;
}

void
Server::checkServerName(std::vector<std::string>& vec) {
	GPS;
	/*
	 * When you set server_name default; in an Nginx server block, it means that this block will respond
	 * to requests that do not match any other server_name specified in the server configuration.
	 * */
	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples server names");

	this->server_name = vec[0];
}

void
Server::checkRoot(std::vector<std::string>& vec) {
	GPS;
	struct stat buf;

	// Assuming only onde path
	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples Root paths");
	std::string path(vec[0].substr(0, vec[0].find(';')));

	if (stat(path.c_str(), &buf) != 0)
		throw std::runtime_error("Error: Root path doesn't exist");
	this->root = path;

	// Handling Index directive here because it depends on the root.
	if (!this->index.empty()) {
		if (stat(std::string(this->root + "/" + this->index).c_str(), &buf) != 0)
		throw std::runtime_error("Error: " + this->index + " doesn't exist");
	}
}

void
Server::checkIndex(std::vector<std::string>& vec) {
	GPS;
	struct stat	buf;

	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples indes values");
	std::string path(vec[0].substr(0, vec[0].find(';')));
	this->index = path;
	if (!this->root.empty()) {
		if (stat(std::string(this->root + "/" + this->index).c_str(), &buf) != 0)
			throw std::runtime_error("Error: " + this->index + " doesn't exist");
	}
}

void
Server::checkAutoIndex(std::vector<std::string>& vec) {
	GPS;
	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples Auto index options");
	if (vec[0] == "on;" || vec[0] == "off;") {
		this->auto_index = (vec[0] == "on;");
		return ;
	}
	throw std::runtime_error("Error: Invalid auto index option. Lower case only");
}

void
Server::checkAllowMethods(std::vector<std::string>& vec) {
	GPS;
	if (vec.empty())
		throw std::runtime_error("DEBUG: Allow methods values is empty. MUST FIX");
	for (size_t i = 0; i < vec.size(); i++) {
		std::string method = (i == vec.size() - 1) ? vec[i].substr(0, vec[i].find(';')) : vec[i];

		if (method == "GET" || method == "POST" || method == "DELETE")
			this->allow_methods.push_back(method);
		else
			throw std::runtime_error("Error: Invalid allow method: " + method);
	}
	// Verify if there is any repeated method
	std::vector<std::string> sortedVec(this->allow_methods);
	std::sort(sortedVec.begin(), sortedVec.end());
	if (std::adjacent_find(sortedVec.begin(), sortedVec.end()) != sortedVec.end())
		throw std::runtime_error("Error: Allow methods directive has duplicated values");
}

void
Server::checkClientMaxBodySize(std::vector<std::string>& vec) {
	if (vec.size() != 1)
		throw std::runtime_error("Error: Invalid number of arguments Client Max Body Size");
	if (vec[0][vec[0].size() - 2] != 'M')
		throw std::runtime_error("Error: Missing type value");
	for (size_t i = 0; i < vec[0].size(); i++) {
		if (vec[0][i] == 'M' && (i != (vec[0].size() - 2)))
			throw std::runtime_error("Error: invalid argument " + vec[0]);
		if ((vec[0][i] < '0' || vec[0][i] > '9') && vec[0][i] != 'M') {
			if (vec[0][i] == ';' && (i == vec[0].size() - 1))
				break;
			throw std::runtime_error("Error: AAAAAAA invalid argument " + vec[0]);
		}
	}
	int value = (vec[0].find('M') != std::string::npos) ? atoi(vec[0].substr(0, vec[0].find('M')).c_str()) : atoi(vec[0].c_str());
	if (value < 0 || value > 65535)
		throw std::runtime_error("Error: Client Max Body Size out of bound");
	this->cMaxBodySize = (uint16_t)value;
}

void
Server::checkErrorPage(std::vector<std::string>& vec) {
	struct stat buf;

	if (vec.size() != 2)
		throw std::runtime_error("Error: Invalid Error Page arguments");
	if (!isStringNum(vec[0]))
		throw std::runtime_error("Error: The error code must be numerical: " + vec[0]);
	if (stat(vec[1].c_str(), &buf) != 0)
		throw std::runtime_error("Invalid Error Page Path");
}


//	Getters
std::map<std::string, std::vector<std::string> >&
Server::getServerDirectives(void) {
	return (_serverDirectives);
}

std::map<std::string, std::map<std::string, std::vector<std::string> > >&
Server::getLocationDirectives(void) {
	return (_locationDirectives);
}

//	Static
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