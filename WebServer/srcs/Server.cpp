/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 16:30:41 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/09 14:09:37 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//Prototypes:
static std::string defaultServerConfig(int);
static std::string defaultLocationConfig(int);


Server::Server(void) {}

Server::~Server(void) {}

Server::Server(std::map<std::string, std::vector<std::string> >& server, std::map<std::string,
			   std::map<std::string, std::vector<std::string> > >& location, std::string& pwd)
			   : _serverDirectives(server), _locationDirectives(location), _pwd(pwd) {
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
			// Directive[i] not in the server block
			if (it_location->second.find(Parser::location_directives[i]) == it_location->second.end()) {
				this->_locationDirectives[it_location->first][Parser::location_directives[i]] = splitStringToVector(
						defaultLocationConfig(i));
			}
		}
	}
}

void
Server::validateServerDirectives(void) {
	GPS;
	std::map<std::string, std::vector<std::string> >& server = this->_serverDirectives;
	std::map<std::string, std::map<std::string, std::vector<std::string> > >& location = this->_locationDirectives;
	std::map<std::string, std::vector<std::string> >::iterator it;
	std::map<std::string, std::map<std::string, std::vector<std::string> > >::iterator ut;

	for (it = server.begin(); it != server.end(); it++) {
		if (it->first == "listen") this->checkListen(it->second);
		else if (it->first == "server_name") this->checkServerName(it->second);
		else if (it->first == "error_page") this->checkErrorPage(it->second);
	}
	for (ut = location.begin(); ut != location.end(); ut++) {
		t_location temp;
		temp.location_name = ut->first;

		// The initialization order matter
		for (int i = 0; Parser::location_directives[i]; i++) {
			it = location[ut->first].find(Parser::location_directives[i]);

			// root = pwd + root
			if (it->first == "root") this->checkRoot(it->second, temp);
			// index = root + location + index
			else if (it->first == "index") this->checkIndex(it->second, temp);
			else if (it->first == "auto_index") this->checkAutoIndex(it->second, temp);
			else if (it->first == "client_max_body_size") this->checkClientMaxBodySize(it->second, temp);
			else if (it->first == "allow_methods") this->checkAllowMethods(it->second, temp);
			else if (it->first == "cgi_pass") this->checkCgi(it->second, temp);
			else this->checkRedirect(it->second, temp);
		}
		this->locations.push_back(temp);
	}
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

	// ============= <DEBUG> ================
	int optval = 1;
	if (setsockopt(this->server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
		throw std::runtime_error("Error: reuse addr error.");
	// ============= </DEBUG> ===============

	// The len parameter specifies the size of the address structure passed as the second argument (sockaddr* addr).
	if (bind(this->server_sock, (sockaddr *)(&this->server_address), sizeof(this->server_address)) < 0)
		throw std::runtime_error(std::string("Error: Couldn't bind socket ") + std::strerror(errno));
	if (listen(this->server_sock, SOMAXCONN) < 0)
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
		if (this->s_host == "127.0.0.1")
			this->s_host = "0.0.0.0";
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
	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples server names");

	this->server_name = vec[0].substr(0, vec[0].find(';'));
	if (this->server_name.empty())
		throw std::runtime_error("Error: Server Name is empty");
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
	std::string error_path_path = vec[1].substr(0, vec[1].find(';'));
	std::string path(this->_pwd + error_path_path);

	if (stat(path.c_str(), &buf) != 0)
		throw std::runtime_error("Error: Error_page file doesn't exist");
	this->error_page[(int)number] = vec[1];
}

// Location Directives Parser
void
Server::checkRoot(std::vector<std::string>& vec, t_location& location) {
	struct stat buf;

	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples Root paths");
	std::string root((vec[0].substr(0, vec[0].find(';'))));
	std::string path(this->_pwd + root);

	if (stat(path.c_str(), &buf) != 0)
		throw std::runtime_error("Error: Location root path doesn't exist");
	location.root = path;
}

void
Server::checkIndex(std::vector<std::string>& vec, t_location& location) {
	//struct stat	buf;

	if (vec.size() != 1)
		throw std::runtime_error("Error: Location has multiples index values");
	std::string index(vec[0].substr(0, vec[0].find(';')));
	std::string complement = ((location.location_name == "/") ? "" : "/");
	std::string path(location.root + location.location_name + complement + index);

	// If auto_index and index not exists in path then listing
	//if (stat(path.c_str(), &buf) != 0)
		//throw std::runtime_error(std::string("Error: Location index path doesn't exist " + location.location_name));
	location.index = path;
}

void
Server::checkAutoIndex(std::vector<std::string>& vec,  t_location& location) {
	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples Auto index options");
	if (vec[0] == "on;" || vec[0] == "off;") {
		location.auto_index = (vec[0] == "on;");
		return ;
	}
	throw std::runtime_error("Error: Location auto_index is invalid");
}

void
Server::checkClientMaxBodySize(std::vector<std::string>& vec, t_location& location) {
	if (vec.size() != 1)
		throw std::runtime_error("Error: Invalid number of arguments location client_max_body_size");
	size_t size = vec[0].size();

	//The last char was already check, and it must be ';'
	//10M;
	if (vec[0][size - 2] != 'M')
		throw std::runtime_error("Error: Missing type value on location client_max_body_size");
	for (size_t i = 0; i < size - 2; i++) {
		if (!std::isdigit(vec[0][i]))
			throw std::runtime_error("Error: invalid argument " + vec[0]);
	}
	long value = std::atoi(vec[0].substr(0, vec[0].find('M')).c_str());
	if (value < 1 || value > 10)
		throw std::runtime_error("Error: Client Max Body Size out of bound");
	location.CMaxBodySize = (short)value;
}

void
Server::checkAllowMethods(std::vector<std::string>& vec, t_location& location) {
	if (vec.empty())
		throw std::runtime_error("Error: Location allow_methods is empty");
	std::vector<std::string> temp;

	for (size_t i = 0; i < vec.size(); i++) {
		std::string method = (i == vec.size() - 1) ? vec[i].substr(0, vec[i].find(';')) : vec[i];

		if (method == "GET" || method == "POST" || method == "DELETE")
			temp.push_back(method);
		else
			throw std::runtime_error("Error: Location invalid allow_method: " + method);
	}
	// Verify if there is any repeated method
	std::sort(temp.begin(), temp.end());
	if (std::adjacent_find(temp.begin(), temp.end()) != temp.end())
		throw std::runtime_error("Error: Location allow_methods has duplicated methods");
	location.allow_methods = temp;
}

void
Server::checkCgi(std::vector<std::string>& vec, t_location& location) {
	if (vec.size() != 1)
		throw std::runtime_error("Error: Location cgi_pass syntax");
	struct stat buf;
	const std::string cgi_bin(vec[0].substr(0, vec[0].find(';')));
	const std::string cgi_path(this->_pwd + "/var/www/cgi-bin/" + cgi_bin);

	if (stat(cgi_path.c_str(), &buf) != 0)
		throw std::runtime_error("Error: cgi_pass file doesn't exist");
	location.cgi_pass = cgi_path;
}

void
Server::checkRedirect(std::vector<std::string>& vec, t_location& location) {
	if (vec.size() != 1)
		throw std::runtime_error("Error: Location redirect syntax");
	// Default value for redirect directive. Root location always exists
	if (vec[0] == "/")
		return ;
	// Everything that doesn't start with http:// or https:// is treated as location.
	// 404 is returned if url is not found in locations.
	const std::string& url = vec[0];
	location.redirect_is_extern = false;
	if (url.find("https://") == 0 || url.find("http://") == 0)
		location.redirect_is_extern = true;
	location.redirect = url;
}

// =====================
//        Getters       
// =====================

std::map<std::string, std::vector<std::string> >&
Server::getServer(void) {
	return (_serverDirectives);
}

std::map<std::string, std::map<std::string, std::vector<std::string> > >&
Server::getLocationMap(void) {
	return (_locationDirectives);
}

int
Server::getSocket(void) const {
	return (this->server_sock);
}

std::string
Server::getPWD(void) const {
	return (this->_pwd);
}

std::map<int, std::string>
Server::getErrorMap(void) const {
	return (this->error_page);
}

t_location*
Server::getBestLocation(const std::string& name) {
	t_location *bestMatch = NULL;
	size_t bestMatchLen = 0;

	for (size_t i = 0; i < this->locations.size(); i++) {
		std::string locationName = this->locations[i].location_name;

		if (name.find(locationName) == 0) { // Check if locationName is a prefix
			if (bestMatchLen < locationName.length()) {
				bestMatch = &this->locations[i];
				bestMatchLen = locationName.length();
			}
		}
	}
	return bestMatch;
}

void *
Server::getDirectiveFromLocation(std::vector<t_location> &locations, const std::string& location_name, const std::string& directive) {
	for (size_t i = 0; i < locations.size(); i++) {
		if (locations[i].location_name == location_name) {
			if (directive == "root") return ((void*)&locations[i].root);
			if (directive == "index") return ((void*)&locations[i].index);
			if (directive == "auto_index") return ((void*)&locations[i].auto_index);
			if (directive == "client_max_body_size") return ((void*)&locations[i].CMaxBodySize);
			if (directive == "allow_methods") return ((void*)&locations[i].allow_methods);
			if (directive == "cgi_pass") return ((void*)&locations[i].cgi_pass);
			if (directive == "redirect") return ((void*)&locations[i].redirect);
		}
	}
	throw std::runtime_error("Error: getDirectiveFromLocation() directive not found");
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
defaultLocationConfig(int directive) {
	switch (directive) {
		case 0: return ("/var/www;"); //Root
		case 1: return ("index.html;"); //Index
		case 2: return ("off;"); //Auto_index
		case 3: return ("1M"); //Client_max_body_size
		case 4: return ("GET;"); // Allow_methods
		case 5: return ("PLACEHOLDER;"); //Cgi_pass
		case 6: return ("/"); //Redirect
		default: throw std::runtime_error("Server.cpp defaultLocationConfig Methods");
	}
}
