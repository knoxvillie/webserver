/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 16:30:41 by kfaustin          #+#    #+#             */
/*   Updated: 2024/06/01 15:18:25 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "utils.hpp"

//Prototypes:
static std::string defaultServerConfig(int);
static std::string defaultLocationConfig(int);


Server::Server(void) {}

Server::~Server(void) {}

Server::Server(std::map<std::string, std::vector<std::string> >& server,
			   std::map<std::string, std::map<std::string, std::vector<std::string> > >& location)
			   : _serverDirectives(server), _locationDirectives(location) {
	GPS;
	this->applyServerDirectives();
	this->validateServerDirectives();
	this->startServerSocket();
}

void
Server::applyServerDirectives(void) {
	GPS;
	std::map<std::string, std::vector<std::string> >::const_iterator it_server;
	std::map<std::string,  std::map<std::string, std::vector<std::string> > >::const_iterator it_location;

	for (int i = 0; Parser::server_directives[i]; ++i) {
		it_server = this->_serverDirectives.find(Parser::server_directives[i]);
		// Directive[i] not found in the server block
		if (it_server == this->_serverDirectives.end()) {
			// Add the key into the map, and initialize it with default values.
			// splitStringToVector handles the case that the directive has more than 1 value. (Allow_methods)
			this->_serverDirectives[Parser::server_directives[i]] = Utils::splitStringToVector(defaultServerConfig(i));
		}
	}
	// Check if the server block doesn't have any location defined or didn't define a root.
	if (this->_locationDirectives.empty() || this->_locationDirectives.find("/") == this->_locationDirectives.end())
		this->_locationDirectives["/"]["index"] = Utils::splitStringToVector("index.html");

	for (it_location = this->_locationDirectives.begin(); it_location != this->_locationDirectives.end(); it_location++) {
		for (int i = 0; Parser::location_directives[i]; i++) {
			// Directive[i] not in the location block
			if (it_location->second.find(Parser::location_directives[i]) == it_location->second.end()) {
				this->_locationDirectives[it_location->first][Parser::location_directives[i]] = Utils::splitStringToVector(
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

	for (int i = 0; Parser::server_directives[i]; i++) {
		it = server.find(Parser::server_directives[i]);

		if (it->first == "host") this->checkHost(it->second);
		else if (it->first == "port") this->checkPorts(it->second);
		else if (it->first == "server_name") this->checkServerName(it->second);
		else if (it->first == "error_page") this->checkErrorPage(it->second);
	}
	for (ut = location.begin(); ut != location.end(); ut++) {
		t_location temp;
		temp.location_name = ut->first;

		// The initialization order matters, because some directives depend on others
		for (int i = 0; Parser::location_directives[i]; i++) {
			it = location[ut->first].find(Parser::location_directives[i]);

			// root = Global::pwd + root
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

	for (size_t i = 0; i < this->s_port.size(); i++) {
		int server_sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

		if (server_sock < 0)
			throw std::runtime_error("ERROR - Server: Couldn't create the server socket");

/* 		if (fcntl(server_sock, F_SETFL, O_NONBLOCK) < 0) {
			close (server_sock);
			throw std::runtime_error("ERROR - Server: failed to set socket as nonblocking");
		} */
/* 
		int optval = 1;
		if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) {
			close (server_sock);
			throw std::runtime_error("ERROR - Server: Couldn't allow the socket to binding to multiples ports");
		} */

		this->server_address.sin_port = htons(this->s_port[i]);

		if ((bind(server_sock, (sockaddr *)(&this->server_address), sizeof(this->server_address))) < 0) {
			close (server_sock);
			throw std::runtime_error(std::string("ERROR - Server: Couldn't bind socket: ") + Utils::intToString((int)this->s_port[i]) + " Why? " + std::strerror(errno));
		}

		if (listen(server_sock, SOMAXCONN) < 0) {
			close (server_sock);
			throw std::runtime_error("ERROR - Server: Couldn't listen on port " + Utils::intToString((int)this->s_port[i]));
		}

		this->server_sockets.push_back(server_sock);

		MLOG("Server " << this->s_host << " listening on port: " + Utils::intToString((int)this->s_port[i]));
	}
}

int
Server::acceptConnection(int socket) const {
	GPS;
	int client_sock;
	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);

	std::memset(&client_address, 0, sizeof(client_address));
	client_sock = accept(socket, (sockaddr*)&client_address, (socklen_t*)&client_address_len);
	if (client_sock < 0)
		throw std::runtime_error("Error: Client socket failed");
	return (client_sock);
}

// ======================
//       Parsing       
// ======================

void
Server::checkHost(std::vector<std::string>& vec) {
	std::memset(&this->server_address, 0, sizeof(this->server_address));

	if (vec.size() != 1)
		throw std::runtime_error("ERROR - Server: Invalid number of hosts detected. Expected 1, but received: " + Utils::intToString((int)vec.size()));
	// Removing ';' from the end of the directive value.
	this->s_host = vec[0].substr(0, vec[0].find(';'));

	if (this->s_host == "localhost")
		this->s_host = "127.0.0.1";
	// Redirection to "0.0.0.0" if needed.
	//if (host == "127.0.0.1") host = "0.0.0.0";

	this->server_address.sin_addr.s_addr = htonl(Utils::ipParserHtonl(this->s_host));
	//this->server_address.sin_addr.s_addr = Utils::ipParserHtonl(this->s_host);
	// IPV4 only
	this->server_address.sin_family = AF_INET;
}

void
Server::checkPorts(std::vector<std::string> &vec) {
	char* endptr;
	std::string token;

	// For safety only, the parser do not allow empty directives values
	if (vec.empty())
		throw std::runtime_error("ERROR - Server: Server block has a empty port directive");
	for (size_t i = 0; i < vec.size(); i++) {
		// Handling the last element of the vec that always ends in ';'
		token = vec[i].substr(0, vec[i].find(';'));
		long ip = std::strtol(token.c_str(), &endptr, 10);

		// Check if the port is numerical.
		if (*endptr != '\0')
			throw std::runtime_error("ERROR - Server: Invalid port type. Expected numerical but received: " + token);
		// Check port range
		if (ip < 1024 || ip > 65535)
			throw std::runtime_error("ERROR - Server: Invalid port range. Expected between [1024, 65535] but received: " + token);
		this->s_port.push_back((uint16_t)ip);
	}
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
	if (!Utils::isStringUnsignedInt(vec[0]))
		throw std::runtime_error("Error: The error code must be numerical: " + vec[0]);
	number = std::atoi(vec[0].c_str());

	if (number < 200 || number > 600)
		throw std::runtime_error("Error: Error page code out of bounds");
	std::string error_path_path = vec[1].substr(0, vec[1].find(';'));
	std::string path(Global::pwd + error_path_path);

	if (stat(path.c_str(), &buf) != 0)
		throw std::runtime_error("Error: Error_page file doesn't exist");
	this->error_page[(int)number] = vec[1];
}

// Location Directives Parser
void
Server::checkRoot(std::vector<std::string>& vec, t_location& location) {
	struct stat buf;

	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiple Root paths");
	std::string root((vec[0].substr(0, vec[0].find(';'))));
	std::string path(Global::pwd + root);

	if (stat(path.c_str(), &buf) != 0)
		throw std::runtime_error("Error: Location root path doesn't exist");
	location.root = path;
}

void
Server::checkIndex(std::vector<std::string>& vec, t_location& location) {
	if (vec.size() != 1)
		throw std::runtime_error("Error: Location has multiples index values");
	std::string index(vec[0].substr(0, vec[0].find(';')));
	std::string complement = ((*(location.root).rbegin() == '/') ? "" : "/");
	std::string path(location.root + complement + index);
	location.index = path;
}

void
Server::checkAutoIndex(std::vector<std::string>& vec,  t_location& location) {
	if (vec.size() != 1)
		throw std::runtime_error("Error: Multiples Auto index options");
	if (vec[0] == "true;" || vec[0] == "false;") {
		location.auto_index = (vec[0] == "true;");
		return ;
	}
	throw std::runtime_error("Error: Location auto_index is invalid");
}

void
Server::checkClientMaxBodySize(std::vector<std::string>& vec, t_location& location) {
	if (vec.size() != 1)
		throw std::runtime_error("Error: Invalid number of arguments location client_max_body_size");
	size_t size = vec[0].size();

	if ((vec[0][size - 2] != 'M') && (vec[0][size - 2] != 'B'))
		throw std::runtime_error("Error: Missing type value on location client_max_body_size");
	for (size_t i = 0; i < size - 2; i++) {
		if (!std::isdigit(vec[0][i]))
			throw std::runtime_error("Error: invalid argument " + vec[0]);
	}
	if (vec[0].find('M') != std::string::npos) {
		long value = std::atol(vec[0].substr(0, vec[0].find('M')).c_str());
		if (value < 1 || value > 10)
			throw std::runtime_error("Error: Client Max Body Size out of bound");
		// (2^10 * 10^3) ~ (2^10 + 2^10) : bytes
		location.CMaxBodySize = (1024 * 1000) * value;
	} else {
		long value = std::atol(vec[0].substr(0, vec[0].find('B')).c_str());
		if (value < 1 || value > 100000)
			throw std::runtime_error("Error: Client Max Body Size out of bound");
		location.CMaxBodySize = value;
	}
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

/*
	NOTES:
		- CGI scrips should not be executed has we have them now, we should have a location
		with a name like ".php" and a directive "cgi_pass" that takes a script as an argument
		and execute that script for all requests that fall into that location (in this example all requests that ask for a file with .php)

	- We should have a directory that stores all our CGIs

	Steps to achieve this:
	- Check the location for cgi_pass and store the name of the script
	- Save the location name if it's an extention and compare the requests before searching for an extention
	- ...
*/

void
Server::checkCgi(std::vector<std::string>& vec, t_location& location) {
	if (vec.size() != 1)
		throw std::runtime_error("Error: Location cgi_pass syntax");
	struct stat buf;
	const std::string cgi_bin(vec[0].substr(0, vec[0].find(';')));
	const std::string cgi_path(Global::pwd + "/var/www/cgi-bin/" + cgi_bin);

	if (stat(cgi_path.c_str(), &buf) != 0)
		throw std::runtime_error("Error: cgi_pass file doesn't exist");
	location.cgi_pass = cgi_path;
}

void
Server::checkRedirect(std::vector<std::string>& vec, t_location& location) {
	if (vec.size() != 1)
		throw std::runtime_error("Error: Location redirect syntax");
	location.redirect_is_extern = false;
	location.redirect = "false";
	// Need to check if the location is root, so, root must never redirect.
	if (location.location_name == "/" || vec[0] == "false;")
		return ;
	// Everything that doesn't start with http:// or https:// is treated as location.
	// 404 is returned if url is not found in locations.
	const std::string& url = vec[0].substr(0, vec[0].find(';'));
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

std::vector<int>
Server::getSockets(void) const {
	return (this->server_sockets);
}

std::map<int, std::string>
Server::getErrorMap(void) const {
	return (this->error_page);
}

std::string
Server::getHost(void) const {
	return (this->s_host);
}

std::vector<uint16_t>
Server::getPort(void) const {
	return (this->s_port);
}

t_location*
Server::getBestLocation(Request& request) {
	t_location *bestMatch = NULL;
	size_t bestMatchLen = 0;
	std::string name = request.getURI();

	for (size_t i = 0; i < this->locations.size(); i++) {
		std::string locationName = this->locations[i].location_name;

		if (locationName[0] == '.' && (request.getExtension() == locationName)) { // Means that the location is a cgi location
			request.setCGI();
			return (&this->locations[i]);
		}
		if (name.find(locationName) == 0) { // Check if locationName is a prefix
			if (bestMatchLen < locationName.length()) {
				bestMatch = &this->locations[i];
				bestMatchLen = locationName.length();
			}
		}
	}
	if (bestMatch != NULL)
		MLOG("Location: " << bestMatch->location_name);
	return bestMatch;
}

t_location*
Server::getBestRedir(const std::string& name) {
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

/* std::string
Server::getListen(void) const {
	return ("MUDAR DEPOIS"); // TODO: MUDAR DEPOIS
} */

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
		case 0: return ("127.0.0.1;"); //Host
		case 1: return ("8080;"); //Ports
		case 2: return ("default;"); //Server_name
		case 3: return ("404 /var/www/error_pages;"); //Error_page
		default: throw std::runtime_error("INTERNAL ERROR: defaultServerConfig func. Invalid server directives");
	}
}

std::string
defaultLocationConfig(int directive) {
	switch (directive) {
		case 0: return ("/var/www;"); //Root
		case 1: return ("index.html;"); //Index
		case 2: return ("false;"); //Auto_index
		case 3: return ("1M;"); //Client_max_body_size
		case 4: return ("GET;"); // Allow_methods
		case 5: return (";"); //Cgi_pass
		case 6: return ("false;"); //Redirect
		default: throw std::runtime_error("Server.cpp defaultLocationConfig Methods");
	}
}
