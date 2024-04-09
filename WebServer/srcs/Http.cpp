/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 18:34:53 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/08 13:43:25 by diogmart         ###   ########.fr       */
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
	this->request = std::string(content);
	MLOG(content);
}

void Http::requestParser(void) {
	GPS;
	std::string token;
	std::stringstream ss(this->request);

	if (ss >> token) {
		if (token != "GET" && token != "POST" && token != "DELETE")
			throw std::runtime_error("Error: Invalid HTTP request method");
		this->method = token;
	} else throw std::runtime_error("Error: Can't read the method in the HTTP request line");

	if (ss >> token) {
		this->url = token;
	} else throw std::runtime_error("Error: Can't read URI in HTTP request line");

	if (ss >> token) {
		if (token != "HTTP/1.1")
			throw std::runtime_error("Error: Invalid HTTP version");
		this->http_version = token;
	} else throw std::runtime_error("Error: Can't read the version in HTTP request line");
}

void Http::responseSend(void) {
	std::ostringstream oss;
	std::string content;
	t_location* actual_location;

	// Find the location corresponding to the URL
	actual_location = this->_server->getLocation(this->url);

	// If the location is found in the URL
	if (actual_location != NULL) {
		int statusCode = 200;

		//Check the HTTP request method
		if (this->method == "GET")
			statusCode = getMethod(actual_location, content);
		else if (this->method == "POST")
			statusCode = postMethod(actual_location);


		switch (statusCode) {
			case 200:
				generateResponse(oss, this->http_version, "200 OK", content);
				break;
			case 201:
				generateResponse(oss, this->http_version, "201 Created", content);

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

static void generateResponse(std::ostringstream& oss, const std::string& http_version, const std::string& status_code, const std::string& content) {
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

void Http::generateErrorResponse(std::ostringstream& oss, int error_code) {
	// Find the error page corresponding to the error code
	std::map<int, std::string>::const_iterator error_page_it;
	error_page_it = this->_server->getErrorMap().find(error_code);

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

int
Http::getMethod(const t_location* location, std::string& content) {
		std::stringstream buffer;
		
		MLOG("~~~~~~~~\n   GET\n~~~~~~~~");

		// Open the file corresponding to the request
		std::string file_path = (this->url == location->location_name) ? location->index : (location->root + this->url);
		MLOG("FILE PATH: " + file_path);
		
		std::ifstream file(file_path.c_str());
		if (file.is_open()) {
			// Read the content of the file
			buffer << file.rdbuf();
			content = buffer.str();
			file.close();

			// Generate the HTTP 200 OK response with the content of the file
			return 200;
		} else {
			// File not found, generate a 404 Not Found response
			MLOG("FILE NOT FOUND");
			return 404;
		}
}

int
Http::postMethod(const t_location *location) {
	MLOG("~~~~~~~~\n   POST\n~~~~~~~~");
	
	if (std::find(location->allow_methods.begin(), location->allow_methods.end(), "POST") == location->allow_methods.end())
		return 405; // Status code for method not allowed
	
	std::string file_path = (this->url == location->location_name) ? location->index : (location->root + this->url);
	MLOG("FILE PATH: " + file_path);

	std::ofstream out_file(file_path.c_str(), std::ofstream::app); // Append changes to the file
	if (out_file.is_open()) {
		
		std::string output = this->request.substr(this->request.find("\r\n\r\n") + 4, std::string::npos);
		MLOG("Output: " + output); 
		out_file << "\n==================\n\n";
		out_file << output;
		out_file << std::endl;
		out_file.close();
		
		// Generate the HTTP 200 OK response
		return 201;
	} else {
		// File not found, generate a 404 Not Found response
		MLOG("FILE NOT FOUND");
		return 404;
	}

	return 200;
}

int
Http::deleteMethod(const t_location *location) {
	MLOG("~~~~~~~~\n   DEL\n~~~~~~~~");
	(void)location;
	return 200;
}

