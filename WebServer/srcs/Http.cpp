/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 18:34:53 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/29 13:15:43 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"
#include "Response.hpp"
#include "utils.hpp"
#include "webserv.hpp"

// Prototypes
static std::string dirTypes(const unsigned char&);
static std::string formatSize(const size_t&);

Http::Http(int connection, Server* server) : _clientSock(connection), _server(server) {
/* 	(this->request).server = server;
	this->requestFromClient();
	this->requestParser();
	this->setHeaderAndBody();
	this->fillHeaderMap();
	if (!request.isCGI)
		this->BuildResponse();
	else {
		CgiHandler cgi(request);
	} */
}

Http::~Http() {}

/* 
NOTE:
 Recv doesn't need to be called in a loop if handled properly, as epoll_wait will keep
 returning EPOLLIN in the socket as long as there is more info to read. We just need
 to make sure all the info is received before we handle the connection
*/
void
Http::receiveFromClient(int socket, Request& request) {
	GPS;
	char buf[BUFFER_SIZE] = {0};

	if (recv(socket, buf, BUFFER_SIZE, MSG_DONTWAIT) < 0)
		throw std::runtime_error("Error: Read from client socket");
	request.receiveData(std::string(buf));
	MLOG(buf);
}

// TODO: change the exception to send an error page
void
Http::requestParser(Request& request) {
	GPS;
	std::string token;
	std::stringstream ss(request.getFull());

	if (ss >> token) {
		if (token != "GET" && token != "POST" && token != "DELETE")
			throw std::runtime_error("Error: Invalid HTTP request method");
		request.setMethod(token);
	} else throw std::runtime_error("Error: Can't read the method in the HTTP request line");

	if (ss >> token) {
		request.setURI(token); // Parses the URI and sets it
	} else throw std::runtime_error("Error: Can't read URI in HTTP request line");

	if (ss >> token) {
		if (token != "HTTP/1.1")
			throw std::runtime_error("Error: Invalid HTTP version");
		request.setHttpVersion(token);
	} else throw std::runtime_error("Error: Can't read the version in HTTP request line");
}

// TODO: make this static and implement the use of Reponse class here
void
Http::BuildResponse(Request& request) {
	bool is_redirect = false;
	t_location* best_location;

	Http::requestParser(request);

	// Find the location corresponding to the URL
	best_location = request.server->getBestLocation(request.getURI());

	//Location not found, generate a 404 Not Found response
	if (best_location == NULL) {
		;//this->findErrorPage(404);
		return;
	}
	request.setFilePath(best_location->root + request.getURI());
	// Checking Location Client Max Body Size.
	if (size_t(best_location->CMaxBodySize) < (request.getBody()).size())
		;//this->findErrorPage(403);
	// Handle redirect
	else if (best_location->redirect != "false") {
		is_redirect = true;
		if (best_location->redirect_is_extern)
			; //this->doResponse(best_location->redirect,"text/html", 302, this->_clientSock);
		else {
			best_location = request.server->getBestLocation(best_location->redirect);
			// Location not found to redirect.
			if (best_location == NULL)
				; //this->findErrorPage(404);
			request.setFilePath(best_location->location_name);
			//this->doResponse(request.getFilePath(), "void", 302, this->_clientSock);
		}
	}
	else if (Utils::isDirectory(request.getFilePath()))
		;//this->doDirectoryResponse(best_location, is_redirect);
	else
		;//this->handleMethod(best_location);
}

void
Http::handleMethod(t_location* location) {
	int status_code = 404;
	std::string content;

	// The url is requesting a file
	if (request.method == "GET")
		status_code = this->getMethod(location->allow_methods, content);
	else if (request.method == "POST")
		status_code = this->postMethod(location->allow_methods);

	switch (status_code) {
		case 200:
			// When a css style file is requested.
			if (this->request.file_path.find(".css") != std::string::npos)
				this->doResponse(content, "text/css", status_code, this->_clientSock);
			else
				this->doResponse(content, "text/html", status_code, this->_clientSock);
			break;
		case 201:
			this->doResponse(content, "text/html", 201, this->_clientSock);
			break;
		default:
			this->findErrorPage(status_code);
			break;
	}
}

void
Http::doDirectoryResponse(t_location *location, bool is_redirect) {
	int statusCode;
	struct stat buf;
	std::string content;

	// Check if the location index exists
	if (stat(location->index.c_str(), &buf) == 0) {
		// Index exists so must be sent.
		this->request.file_path = location->index;
		statusCode = getMethod(location->allow_methods, content);
		this->doResponse(content, "text/html", statusCode, this->_clientSock);
	}
	// If auto_index off and the index doesn't exist -> forbidden request
	else if (!location->auto_index)
		this->findErrorPage(403);
	// then listing
	else {
		if (is_redirect)
			this->doResponse(directoryListing(), "text/html", 302, this->_clientSock);
		else
			this->doResponse(directoryListing(), "text/html", 200, this->_clientSock);
	}
}

void
Http::findErrorPage(int status_code) {
	// Find the error page corresponding to the status code
	std::map<int, std::string>::const_iterator it;
	it  = this->_server->getErrorMap().find(status_code);

	// There isn't an error page defined for the specif error
	if (it == this->_server->getErrorMap().end()) {
		std::ostringstream content;

		Utils::createStyleIfNotExists();
		Utils::createGenericErrorPage(content, status_code);
		this->doResponse(content.str(), "text/html", status_code, this->_clientSock);
		return ;
	}
	std::string error_path(this->_server->getErrorMap()[status_code]);
	std::string path(Global::pwd + error_path);
	path = path.substr(0, path.find(';'));

	// Open the error page
	std::ifstream file(path.c_str());
	if (file.is_open()) {
		// Read the content of the error page
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string content = buffer.str();
		file.close();
		// Generate the HTTP response with the content of the error page
		this->doResponse(content, "text/html", status_code, this->_clientSock);
	} else throw std::runtime_error("Error: Cannot open the error_page file");
}

/*
	Things to change in the findErrorPage() function:
		- Dont throw exceptions just because one server couldn't find an error page, probably just send a standard error
		to the client and move on;
		- Not all errors will be "Not Found", only 404 is. Read https://datatracker.ietf.org/doc/html/rfc2616#autoid-45 for more info
*/

std::string
Http::directoryListing(void) {
	DIR* dir;
	struct dirent* entry;
	struct stat file_stat;
	std::stringstream html;

	dir = opendir(this->request.file_path.c_str());
	if (!dir) {
		MLOG("ERROR: Unable to open directory");
		return "<html><head><title>Error</title></head><body><h1>Error opening directory.</h1></body></html>";
	}
	Utils::createListingPage(html, this->request.file_path);
	while ((entry = readdir(dir))) {
		if (std::strcmp(entry->d_name, ".") == 0 ) continue; // Skip hidden current directory markers.
		std::string filePath = this->request.file_path + "/" + entry->d_name;
		if(stat(filePath.c_str(), &file_stat) == 0) {
			char time_buf[100];
			std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_ctime));
			html << "<tr><td><a href='" << ((entry->d_type == DT_DIR) ? (std::string(entry->d_name) + "/") : entry->d_name) << "'>" << entry->d_name << "</a></td>"
				 << "<td>" << dirTypes(entry->d_type) << "</td>"
				 << "<td>" << time_buf << "</td>"
				 << "<td>" << formatSize(file_stat.st_size) << "</td></tr>";
		}
	}
	closedir(dir);
	html << "</table></body></html>";
	return html.str();
}

void
Http::doResponse(const std::string& content, const std::string& type, int status_code, int& clientSock) {
	std::ostringstream oss;
	if (status_code == 302) {
		oss << "HTTP/1.1" << " " << status_code << "\r\n";
		oss << "Cache-Control: no-cache, private\r\n";
		oss << "Location: " << content << "\r\n";
		oss << "\r\n";
	}
	oss << "HTTP/1.1" << " " << status_code << "\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: " << type << "\r\n";
	oss << "Content-Length: " << content.length() << "\r\n";
	oss << "\r\n";
	oss << content;

	// Send the response to the client
	if (send(clientSock, oss.str().c_str(), oss.str().length(), MSG_DONTWAIT) < 0) {
		throw std::runtime_error("Error: send function failed");
	}
}

int
Http::getMethod(const std::vector<std::string>& methods, std::string& content) {
	if (std::find(methods.begin(), methods.end(), "GET") != methods.end()) {
		std::stringstream buffer;
		int flag = Utils::isRegularFile(this->request.file_path);
		if (flag == 1) {
			std::ifstream file(this->request.file_path.c_str());

			if(!file.is_open())
				return (500);
			buffer << file.rdbuf();
			content = buffer.str();
			file.close();
			return (200);
		}
		// The file doesn't exist.
		if (flag == 0) return (404);
		// The request is a directory, special device or a symbolic link.
		if (flag == -1) return (400);
	}
	// Method not allowed on location
	return (405);
}

int
Http::postMethod(const std::vector<std::string>& methods) {
	std::ofstream out_file(request.file_path.c_str(), std::ofstream::app);

	// Status code for method not allowed
	if (std::find(methods.begin(), methods.end(), "POST") == methods.end())
		return (405);

	MLOG("~~~~~~~~\n   POST\n~~~~~~~~");


	if (!out_file) {
		// No such file or directory
		if (errno == ENOENT)
			return (404);
			// Permission denied
		else if (errno == EACCES)
			return (403);
			// Internal Server Error
		else
			return (500);
	}
	std::string output = this->request.content.substr(this->request.content.find("\r\n\r\n") + 4, std::string::npos);

	MLOG("Output: " + output);
	out_file << "\n**************\n\n";
	out_file << output;
	out_file << std::endl;
	out_file.close();

	// Generate the HTTP 200 OK response
	return (201);
}

int
Http::deleteMethod(const t_location *location) {
	MLOG("~~~~~~~~\n   DEL\n~~~~~~~~");

	if (std::find(location->allow_methods.begin(), location->allow_methods.end(), "DELETE") == location->allow_methods.end())
		return 405; // Status code for method not allowed

	(void)location;
	return 200;
}

// Static
static std::string
dirTypes(const unsigned char& type) {
	switch (type) {
		case DT_BLK: return ("block device");
		case DT_CHR: return ("character device");
		case DT_DIR: return ("directory");
		case DT_FIFO: return ("named pipe (FIFO");
		case DT_LNK: return ("symbolic link");
		case DT_REG: return ("regular file");
		case DT_SOCK: return ("UNIX domain socket");
		default: return ("unknown");
	}
}

static std::string
formatSize(const size_t& size) {
	std::stringstream sstream;
	if (size < 1024) sstream << size << " B";
	else if (size < 1024 * 1024) sstream << size / 1024 << " KB";
	else if (size < 1024 * 1024 * 1024) sstream << size / (1024 * 1024) << " MB";
	else sstream << size / (1024 * 1024 * 1024) << " GB";
	return (sstream.str());
}

void 
Http::decodeURI() {
  
	std::string	newUri;

	for (std::string::iterator iter = this->_decodedURI.begin(); iter != this->_decodedURI.end(); ++iter) 
	{
		if (*iter == '%' && iter + 1 != this->_decodedURI.end() && iter + 2 != this->_decodedURI.end())
		{
			char numberChar[3] = {*(iter + 1), *(iter + 2), 0};
			char *endptr = NULL;
			u_int64_t numberInt = std::strtoul(numberChar, &endptr, 16);
			if (endptr && *endptr == 0)
			{
				newUri += static_cast<char>(numberInt);
				iter += 2;
			}
		} else {
			newUri += *iter;
		}
	}
	this->request.url = newUri;
}
