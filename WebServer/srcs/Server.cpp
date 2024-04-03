/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 16:30:41 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/03 11:17:28 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//Prototypes:
static std::string defaultServerConfig(int);
static std::string defaultLocationConfig(int, const std::string&, const std::string&);


Server::Server(void) {}

Server::~Server(void) {}

Server::Server(std::map<std::string, std::vector<std::string> >& server, std::map<std::string,
			   std::map<std::string, std::vector<std::string> > >& location)
			   : _serverDirectives(server), _locationDirectives(location) {
	GPS;
	this->applyServerDirectives();
	this->validateServerDirectives();
	this->startServerSocket();
}

// This method stands to check which "essentials directives" are not in the server block and initialize it.
void
Server::applyServerDirectives(void) {
	GPS;
	std::map<std::string, std::vector<std::string> >::const_iterator it_server;
	std::map<std::string,  std::map<std::string, std::vector<std::string> > >::const_iterator it_location;

	(void) it_location;
	for (int i = 0; Parser::server_directives[i]; ++i) {
		it_server = this->_serverDirectives.find(Parser::server_directives[i]);
		// Directive[i] not in the server block
		if (it_server == this->_serverDirectives.end()) {
			// Add the key into the map, and initialize it with default values.
			// splitStringToVector handles the case that the directive has more than 1 value. (Allow_methods)
			this->_serverDirectives[Parser::server_directives[i]] = splitStringToVector(defaultServerConfig(i));
		}
	}
	if (this->_locationDirectives.empty() || this->_locationDirectives.find("/") == this->_locationDirectives.end())
		this->_locationDirectives["/"]["index"] = splitStringToVector("index.html");

	for (it_location = this->_locationDirectives.begin(); it_location != this->_locationDirectives.end(); it_location++) {
		for (int i = 0; Parser::location_directives[i]; i++) {
			if (it_location->second.find(Parser::location_directives[i]) == it_location->second.end()) {
				this->_locationDirectives[it_location->first][Parser::location_directives[i]] = splitStringToVector(
						defaultLocationConfig(i, it_location->first, this->root));
			}
		}
	}
}

void
Server::validateServerDirectives(void) {
	GPS;
	std::map<std::string, std::vector<std::string> >& server = this->_serverDirectives;
	std::map<std::string, std::map<std::string, std::vector<std::string> > >& location = this->_locationDirectives;
	for (std::map<std::string, std::vector<std::string> >::iterator it = server.begin(); it != server.end(); it++)
		this->directiveSelector(it->first, it->second, true);
	for (std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator it = location.begin(); it != location.end(); it++)
		this->directiveSelector(it->second.begin()->first, it->second.begin()->second, false);
}

// ======================
//      Connections       
// ======================

void
Server::startServerSocket(void) {
	GPS;
	this->server_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (this->server_sock < 0)
		throw std::runtime_error("Error: Couldn't create socket");
	// The len parameter specifies the size of the address structure passed as the second argument (sockaddr* addr).
	if (bind(this->server_sock, (sockaddr *)(&this->server_address), sizeof(this->server_address)) < 0)
		throw std::runtime_error("Error: Couldn't bind socket");
	if (listen(this->server_sock, 42) < 0) //SOMAXCONN
		throw std::runtime_error("Error: Couldn't listen");
}

int
Server::acceptConnection(void) const {
	GPS;
	int client_sock;
	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);

	std::memset(&client_address, 0, sizeof(client_address));
	client_sock = accept(this->server_sock, (sockaddr*)&client_address, (socklen_t*)&client_address_len);
	if (client_sock < 0)
		throw std::runtime_error("Error: Client socket failed");
	return (client_sock);
}

// ======================
//       Parsing       
// ======================

void
Server::directiveSelector(const std::string& directive, std::vector<std::string>& vec, bool server_block) {
	if (directive == "listen")
		this->checkListen(vec);
	else if (directive == "server_name")
		this->checkServerName(vec);
	else if(directive == "root")
		this->checkRoot(vec, server_block);
	else if (directive == "index")
		this->checkIndex(vec, server_block);
	else if (directive == "auto_index")
		this->checkAutoIndex(vec, server_block);
	else if (directive == "allow_methods")
		this->checkAllowMethods(vec, server_block);
	else if (directive == "client_max_body_size")
		this->checkClientMaxBodySize(vec);
	else if (directive == "error_page")
		this->checkErrorPage(vec);
	else
		throw std::runtime_error("Error: Directive is not valid ");
}

//	Directives parser
void
Server::checkListen(std::vector<std::string>& vec) {
	int s_port;

	std::memset(&this->server_address, 0, sizeof(this->server_address));
	if (vec.size() > 1)
		throw std::runtime_error("Error: listen directive has too many arguments");
	std::string token(vec[0]);
	// ':' not in token
	if (token.find(':') == std::string::npos) {
		this->s_host = "0.0.0.0";
		this->server_address.sin_addr.s_addr = INADDR_ANY;
		s_port = std::atoi(token.substr(0, token.find(';')).c_str());
	} else {
		size_t	pos = token.find(':');
		this->s_host = token.substr(0, pos);
		s_port = (std::atoi(token.substr(pos + 1).c_str()));
		this->server_address.sin_addr.s_addr = ::ipParserHtonl(this->s_host);
	}
	if (s_port < 1024 || s_port > 65535)
		throw std::runtime_error("Error: Server s_port out of range [1024, 65535]");
	this->s_port = (uint16_t)s_port;
	this->server_address.sin_family = AF_INET;
	this->server_address.sin_port = htons(this->s_port);
}

void
Server::checkServerName(std::vector<std::string>& vec) {
	/*
	 * When you set server_name default; in an Nginx server block, it means that this block will respond
	 * to requests that do not match any other server_name specified in the server configuration.
	 * */
	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples server names");

	this->server_name = vec[0].substr(0, vec[0].find(';'));
	if (this->server_name.empty())
		throw std::runtime_error("Error: Server Name is empty");
}

void
Server::checkRoot(std::vector<std::string>& vec, bool server_block) {
	struct stat buf;

	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples Root paths");
	std::string path(vec[0].substr(0, vec[0].find(';')));

	if (stat(path.c_str(), &buf) != 0)
		throw std::runtime_error("Error: Root path doesn't exist");

	if (server_block) {
		this->root = path;

		// Handling Index directive here because it depends on the root.
		if (!this->index.empty()) {
			if (stat(std::string(this->root + "/" + this->index).c_str(), &buf) != 0)
				throw std::runtime_error("Error: " + this->index + " doesn't exist");
		}
	}
}

void
Server::checkIndex(std::vector<std::string>& vec, bool server_block) {
	struct stat	buf;

	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples index values");
	std::string path(vec[0].substr(0, vec[0].find(';')));

	if (server_block) {
		this->index = path;

		if (!this->root.empty()) {
			if (stat(std::string(this->root + "/" + this->index).c_str(), &buf) != 0)
				throw std::runtime_error("Error: " + this->index + " doesn't exist");
		}
	}
}

void
Server::checkAutoIndex(std::vector<std::string>& vec,  bool server_block) {
	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples Auto index options");
	if (vec[0] == "on;" || vec[0] == "off;") {
		if (server_block)
			this->auto_index = (vec[0] == "on;");
		return ;
	}
	throw std::runtime_error("Error: Invalid auto index option. Lower case only");
}

void
Server::checkAllowMethods(std::vector<std::string>& vec,  bool server_block) {
	if (vec.empty())
		throw std::runtime_error("Error: Allow methods values is empty");
	std::vector<std::string> temp;

	for (size_t i = 0; i < vec.size(); i++) {
		std::string method = (i == vec.size() - 1) ? vec[i].substr(0, vec[i].find(';')) : vec[i];

		if (method == "GET" || method == "POST" || method == "DELETE")
			temp.push_back(method);
		else
			throw std::runtime_error("Error: Invalid allow method: " + method);
	}
	// Verify if there is any repeated method
	std::sort(temp.begin(), temp.end());
	if (std::adjacent_find(temp.begin(), temp.end()) != temp.end())
		throw std::runtime_error("Error: Allow methods directive has duplicated values");
	if (server_block)
		this->allow_methods = temp;
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
			throw std::runtime_error("Error: Invalid argument " + vec[0]);
		}
	}
	long value = std::atoi(vec[0].substr(0, vec[0].find('M')).c_str());
	if (value < 1 || value > 1024)
		throw std::runtime_error("Error: Client Max Body Size out of bound");
	this->lCMaxBodySize = (uint16_t)value;
}

void
Server::checkErrorPage(std::vector<std::string>& vec) {
	struct stat buf;
	long number;

	if (vec.size() != 2)
		throw std::runtime_error("Error: Invalid Error Page arguments");
	if (!isStringUnsignedInt(vec[0]))
		throw std::runtime_error("Error: The error code must be numerical: " + vec[0]);
	number = std::atoi(vec[0].c_str());

	if (number < 200 || number > 600)
		throw std::runtime_error("Error: Error page code out of bounds");
	if (stat(vec[1].substr(0, vec[1].find(';')).c_str(), &buf) != 0)
		throw std::runtime_error("Error: Error_page file doesn't exist");
	this->error_page[(int)number] = vec[1];
}

// =====================
//        Getters       
// =====================

std::map<std::string, std::vector<std::string> >&
Server::getServer(void) {
	return (_serverDirectives);
}

std::map<std::string, std::map<std::string, std::vector<std::string> > >&
Server::getLocation(void) {
	return (_locationDirectives);
}

int
Server::getSocket(void) const {
	return (this->server_sock);
}

std::string
Server::getRoot(void) const {
	return (this->root);
}

std::string
Server::getIndex(void) const {
	return (this->index);
}

std::map<int, std::string>
Server::getErrorMap(void) const {
	return (this->error_page);
}

//	Static
std::string
defaultServerConfig(int directive) {
	switch (directive) {
		case 0: return ("0.0.0.0:8080;"); //Listen
		case 1: return ("default;"); //Server_name
		case 2: return ("/var/www/error_pages;"); //Error_page
		default: throw std::runtime_error("Server.cpp defaultServerConfig invalid server directives");
	}
}

std::string
defaultLocationConfig(int directive, const std::string& location, const std::string& server_root) {
	switch (directive) {
		case 0: return ("index.html;"); //Index
		case 1: return (std::string(server_root + location + ";")); //Root
		case 2: return ("on;"); //Auto_index
		case 3: return ("GET POST DELETE;"); // Allow_methods
		case 4: return ("no clue;"); //Cgi_pass
		default: throw std::runtime_error("Server.cpp defaultLocationConfig Methods");
	}
}

