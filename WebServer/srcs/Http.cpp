/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 18:34:53 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/12 15:16:51 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"

/*
    Host: Specifies the host and s_port to which the request is being sent. In this case, it's "0.0.0.0:8080".
    User-Agent: Provides information about the client making the request. In this case, it indicates that the user agent is Mozilla Firefox running on Ubuntu Linux.
    Accept: Indicates what media types the client can understand. It prefers HTML but also accepts other formats like XML and images.
    Accept-Language: Specifies the preferred language for the response. In this case, it prefers English.
    Accept-Encoding: Indicates the types of content encodings that the client supports. Here, it supports gzip and deflate compression methods.
    Connection: Specifies whether the connection should be kept alive after the current request/response cycle.
    Upgrade-Insecure-Requests: Indicates that the client would like the server to upgrade the request to a secure HTTPS connection if possible.*/

// Prototypes
static void generateResponse(std::ostringstream&, const std::string&, const std::string&, const std::string&);


Http::Http(int connection, Server* server) : _client(connection), _server(server) {
	this->requestFromClient();
	this->requestParser();
	this->responseSend();
}

Http::~Http() {}

void Http::requestFromClient() {
	GPS;
	char content[BUFFER_SIZE] = {0};

	// Multiple recv calls might be needed for a request bigger than BUFFER_SIZE no ?
	// also BUFFER_SIZE value was just a placeholder, might need to be changed
	if (recv(this->_client, content, BUFFER_SIZE, MSG_DONTWAIT) < 0) 
		throw std::runtime_error("Error: Read from client socket");
	(this->request).full = std::string(content);
	MLOG(content);
	setHeaderAndBody(); // not sure if it should be called here
}

void Http::requestParser(void) {
	GPS;
	std::string token;
	std::stringstream ss((this->request).full);

	if (ss >> token) {
		if (token != "GET" && token != "POST" && token != "DELETE")
			throw std::runtime_error("Error: Invalid HTTP request method");
		(this->request).method = token;
	} else throw std::runtime_error("Error: Can't read the method in the HTTP request line");

	if (ss >> token) {
		(this->request).url = token;
	} else throw std::runtime_error("Error: Can't read URI in HTTP request line");

	if (ss >> token) {
		if (token != "HTTP/1.1")
			throw std::runtime_error("Error: Invalid HTTP version");
		this->http_version = token;
	} else throw std::runtime_error("Error: Can't read the version in HTTP request line");
}

static void
generateResponse(std::ostringstream& oss, const std::string& http_version, const std::string& status_code, const std::string& content) {
	if (status_code == "201 Created") {
		oss << "HTTP/1.1 201 Created\r\n";
		oss << "Location: http://0.0.0.0:8080/FormInputs/followers.txt\r\n";
		oss << "Cache-Control: no-cache, private\r\n";
		oss << "Content-Type: application/json\r\n";
		oss << "Content-Length: 75\r\n";
		oss << "\r\n";
		oss << "{";
  		oss << "\"status\": \"success\",";
  		oss << "\"message\": \"Your request was processed successfully.\"";
		oss << "}";
	}
	else {
		oss << http_version << " " << status_code << "\r\n";
		oss << "Cache-Control: no-cache, private\r\n";
		oss << "Content-Type: text/html\r\n";
		oss << "Content-Length: " << content.length() << "\r\n";
		oss << "\r\n";
		oss << content;
	}
}

void
Http::generateErrorResponse(std::ostringstream& oss, int error_code) {
	// Find the error page corresponding to the error code
	std::map<int, std::string>::const_iterator error_page_it;
	error_page_it = this->_server->getErrorMap().find(error_code);

	// There is an error page defined for the specif error
	if (error_page_it != this->_server->getErrorMap().end()) {
		std::string error_path = this->_server->getErrorMap()[error_code];
		std::string path(this->_server->getPWD() + error_path);
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
			generateResponse(oss, this->http_version, intToString(error_code) + " Not Found", content);
		} else {
			throw std::runtime_error("Error: Cannot open the error_page file");
		}
	} else {
		throw std::runtime_error("Error: Error page not found for code " + intToString(error_code));
	}
}

/*
	Things to change in the generateErrorResponse() function:
		- Dont throw exceptions just because one server couldn't find an error page, probably just send a standard error
		to the client and move on;
		- Not all errors will be "Not Found", only 404 is. Read https://datatracker.ietf.org/doc/html/rfc2616#autoid-45 for more info
*/

std::string
Http::directoryListing(void) {
	DIR* dir;
	struct dirent* entry;
	std::stringstream html;
	std::vector<std::string> file_content;

	MLOG("PATH-> " + request.file_path);
	dir = opendir(this->request.file_path.c_str());
	if (dir) {
		entry = readdir(dir);
		while (entry) {
			// Directory content different from the file itself
			if (std::strcmp(entry->d_name, ".") != 0) {	
				std::string result = entry->d_name;
				
				if (entry->d_type == DT_DIR)
					result += "/"; 
				file_content.push_back(result);
			}
			entry = readdir(dir);
		}
		closedir(dir);
	} else
		MLOG("ERROR: Unable to open directory");
	html << "<html><head><title>Directory listing</title></head><body>"
		 << "<h1>Index of " << this->request.file_path << "</h1><ul>";
	for (size_t i = 0; i < file_content.size(); i++)
		html << "<li><a href=\"" << file_content[i] << "\">" << file_content[i] << "</a></li>";
	html << "</ul></body></html>";
	return (html.str());
}


void
Http::responseSend(void) {
	int statusCode;
	struct stat buf;
	std::string content;
	std::ostringstream oss;
	t_location* actual_location;

	// Find the location corresponding to the URL
	statusCode = 404;
	actual_location = (_server)->getLocation(request.url);

	// If the location is found in the URL
	if (actual_location != NULL) {
		request.file_path = actual_location->root + request.url;

		// It is a location or a directory
		if (request.file_path[request.file_path.size() - 1] == '/') {
			MLOG("DIR REQUESTED");
			// Check if the location index exists
			if (stat(actual_location->index.c_str(), &buf) == 0) {
				request.file_path = actual_location->index;
				statusCode = getMethod(actual_location->allow_methods, content);
			}
			// If auto_index off or the directory doesn't exist and the index file cannot be opened
			else if (!actual_location->auto_index || (stat(request.file_path.c_str(), &buf) != 0)) {
				MLOG("Autoindex off or the path doesn't exist");
				generateErrorResponse(oss, 403); // Forbidden request
				return;
			}
			else
				generateResponse(oss, http_version, "200", directoryListing());
		} // file_path must be a file, and it will be checked on the response.
		else if (request.method == "GET")
			statusCode = getMethod(actual_location->allow_methods, content);
		else if (request.method == "POST")
			statusCode = postMethod(actual_location->allow_methods);


		switch (statusCode) {
			case 200:
				generateResponse(oss, http_version, "200 OK", content);
				break;
			case 201:
				generateResponse(oss, http_version, "201 Created", content);
				break;
			default:
				generateErrorResponse(oss, statusCode);
				break;
		}
	} else {
		MLOG("LOCATION NOT FOUND");
		// Location not found, generate a 404 Not Found response
		generateErrorResponse(oss, 404);
	}

	// Get the generated response
	std::string response = oss.str();

	// Send the response to the client
	if (send(this->_client, response.c_str(), response.length(), 0) < 0) {
		throw std::runtime_error("Error: send function failed");
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
	std::string output = this->request.full.substr(this->request.full.find("\r\n\r\n") + 4, std::string::npos);

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
	std::string content = request.full;

	request.first_line = content.substr(0, content.find("\r\n"));
	MLOG("\n\n\n\nFIRST LINE: " + request.first_line);

	request.header = content.substr((request.first_line).length(), content.find("\r\n\r\n"));
	MLOG("\n\n\n\nHEADER: " + request.header);
	
	request.body = content.substr(content.find("\r\n\r\n") + 1, std::string::npos);
	MLOG("\n\n\n\nBODY: " + request.body);
	fillHeaderMap();
	MLOG("\n\n\n\n")
}

void
Http::fillHeaderMap(void) {
	GPS;
	std::string line, header = request.header;
	size_t i = 0, pos = 0;
	
	MLOG("\n\n\n\nHEADER222: " + request.header);

	do {
		pos = header.find("\r\n", pos);
		MLOG(pos);
		
		line = header.substr(i, pos);
		i = pos + 2; // "\r\n" is 2 characters
		if (line.empty())
			break;

		std::string key = line.substr(0, line.find(":"));
		std::string value = line.substr(line.find(":") + 2, std::string::npos); // +2 because of whitespace after ":"
		request.headerMap[key] = value;
	
	} while (pos != std::string::npos);
	

	// printing map for debug
	std::map<std::string, std::string>::const_iterator it;
    for (it = request.headerMap.begin(); it != request.headerMap.end(); ++it) {
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
    }
}


bool
Http::isCGI(const std::string& file) {
	
	std::string extension = file.substr(file.find_last_of('.'), std::string::npos);

	// Check if the extension matches any of our allowed CGIs, then send the request to that
	
	return false;
}
