/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 18:34:53 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/16 16:11:14 by kfaustin         ###   ########.fr       */
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
	this->requestFromClient();
	this->setHeaderAndBody();
	this->fillHeaderMap();
	this->requestParser();
	this->sendResponse();
}

Http::~Http() {}

void Http::requestFromClient() {
	GPS;
	char content[BUFFER_SIZE] = {0};

	if (recv(this->_clientSock, content, BUFFER_SIZE, MSG_DONTWAIT) < 0)
		throw std::runtime_error("Error: Read from client socket");
	this->request.content = std::string(content);
	//MLOG(content);
}

void Http::requestParser(void) {
	GPS;
	std::string token;
	std::stringstream ss(this->request.content);

	if (ss >> token) {
		if (token != "GET" && token != "POST" && token != "DELETE")
			throw std::runtime_error("Error: Invalid HTTP request method");
		(this->request).method = token;
	} else throw std::runtime_error("Error: Can't read the method in the HTTP request line");

	if (ss >> token) {
		this->request.url = token;
	} else throw std::runtime_error("Error: Can't read URI in HTTP request line");

	if (ss >> token) {
		if (token != "HTTP/1.1")
			throw std::runtime_error("Error: Invalid HTTP version");
		this->request.http_version = token;
	} else throw std::runtime_error("Error: Can't read the version in HTTP request line");
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
		this->doResponse(content.str(), status_code, this->_clientSock);
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
		this->doResponse(content, status_code, this->_clientSock);
	} else throw std::runtime_error("Error: Cannot open the error_page file");
}

/*
	Things to change in the findErrorPage() function:
		- Dont throw exceptions just because one server couldn't find an error page, probably just send a standard error
		to the client and move on;
		- Not all errors will be "Not Found", only 404 is. Read https://datatracker.ietf.org/doc/html/rfc2616#autoid-45 for more info
*/
std::string Http::directoryListing(void) {
	DIR* dir;
	struct dirent* entry;
	struct stat file_stat;
	std::stringstream html;

	MLOG("PATH-> " + this->request.file_path);
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
Http::doResponse(const std::string& content, int status_code, int& clientSock) {
	std::ostringstream oss;

	oss << "HTTP/1.1" << " " << status_code << "\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << content.length() << "\r\n";
	oss << "\r\n";
	oss << content;

	// Send the response to the client
	if (send(clientSock, oss.str().c_str(), oss.str().length(), 0) < 0) {
		throw std::runtime_error("Error: send function failed");
	}
}

void
Http::sendResponse(void) {
	int statusCode;
	struct stat buf;
	std::string content;
	t_location* actual_location;

	// Find the location corresponding to the URL
	statusCode = 404;
	actual_location = this->_server->getBestLocation(request.url);

	// If the location is found in the URL
	if (actual_location != NULL) {
		request.file_path = actual_location->root + request.url;

		// It is a location or a directory
		if (request.file_path[request.file_path.size() - 1] == '/') {
			// Check if the location index exists
			if (stat(actual_location->index.c_str(), &buf) == 0) {
				request.file_path = actual_location->index;
				statusCode = getMethod(actual_location->allow_methods, content);
			}
			// If auto_index off or the directory doesn't exist and the index file cannot be opened
			else if (!actual_location->auto_index || (stat(request.file_path.c_str(), &buf) != 0)) {
				this->findErrorPage(403); // Forbidden request
				return;
			}
			else {
				this->doResponse(directoryListing(), 200, this->_clientSock);
				return ;
			}
		}
		if (request.method == "GET")
			statusCode = getMethod(actual_location->allow_methods, content);
		else if (request.method == "POST")
			statusCode = postMethod(actual_location->allow_methods);
		switch (statusCode) {
			case 200:
				this->doResponse(content, 200, this->_clientSock);
				break;
			case 201:
				this->doResponse(content, 201, this->_clientSock);
				break;
			default:
				this->findErrorPage(statusCode);
				break;
		}
	} else { //Location not found, generate a 404 Not Found response
		this->findErrorPage(404);
	}
}

int
Http::getMethod(const std::vector<std::string>& methods, std::string& content) {
	// Method not allowed on location
	if (std::find(methods.begin(), methods.end(), "GET") == methods.end())
		return 405;
	std::stringstream buffer;
	std::ifstream file((request.file_path).c_str());

	MLOG("~~~~~~~~\n   GET\n~~~~~~~~");
	if (!file) {
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
	buffer << file.rdbuf();
	content = buffer.str();
	file.close();
	// Success
	return (200);
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

void
Http::setHeaderAndBody(void) {
	GPS;
	std::string content = request.content;

	this->request.request_line = content.substr(0, content.find("\r\n"));
	this->request.header = content.substr((request.request_line).length() + 2, content.find("\r\n\r\n"));
	this->request.body = content.substr(content.find("\r\n\r\n") + 1, std::string::npos);
}

void
Http::fillHeaderMap(void) {
	GPS;
	std::string line;
	std::string header(request.header);
	size_t pos = 0;

	while ((pos = header.find("\r\n")) != std::string::npos) {
		line = header.substr(0, pos);
		header.erase(0, pos + 2);
		if (line.empty())
			break;
		size_t colPos = line.find(":");

		if (colPos != std::string::npos) {
			std::string key = line.substr(0, colPos);
			std::string value = line.substr(colPos + 2, std::string::npos); // +2 because of whitespace after ":"
			request.headerMap[key] = value;
		}
	}
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
