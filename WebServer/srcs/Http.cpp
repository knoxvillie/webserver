/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 18:34:53 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/03 11:12:46 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 11:56:31 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/01 16:25:19 by kfaustin         ###   ########.fr       */
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
		if (token != "GET" && token != "POST" && token != "DELETE") // Should just check allowed methods from the server no ?
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

/*
1XX (informational)
    The request was received, continuing process.
2XX (successful)
    The request was successfully received, understood, and accepted.
3XX (redirection)
    Further action needs to be taken in order to complete the request.
4XX (client error)
    The request contains bad syntax or cannot be fulfilled.
5XX (server error)
    The server failed to fulfill an apparently valid request.
 * */



//GET /lacrimosa.mp3 HTTP/1.1
//Host: 127.0.0.1:8080
//User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:124.0) Gecko/20100101 Firefox/124.0
//Accept: audio/webm,audio/ogg,audio/wav,audio*/
///*;q=0.9,application/ogg;q=0.7,video/*;q=0.6,*//*
//*;q=0.5
//Accept-Language: en-US,en;q=0.5
//Range: bytes=0-
//			 Connection: keep-alive
//		Referer: http://127.0.0.1:8080/
//Cookie: csrftoken=v2M9Xz9Js2eQqZyi6nzAWVDaoIYuHNOc; username-127-0-0-1-8888="2|1:0|10:1711492418|23:username-127-0-0-1-8888|44:ZDBjOGU0NWFhM2Q0NGM2OGFjOGQ0NTRhOGQwMzhjZjA=|701de7e960daea0b2eb837062378c34a3f11abb2c6b8042089ac3288d4d8fb63"
//Sec-Fetch-Dest: audio
//		Sec-Fetch-Mode: no-cors
//		Sec-Fetch-Site: same-origin
//		Accept-Encoding: identity

void Http::responseSend(void) {
	std::string content;
	std::ostringstream oss;
	std::stringstream buffer;


	// Find the location corresponding to the URL
	std::map<std::string, std::map<std::string, std::vector<std::string> > >::
	        const_iterator it = this->_server->getLocation().find(this->url);

	// If the URL is found in the location
	if (it != this->_server->getLocation().end()) {
		// Extract the index from the location: map[url]->map[index]->vec[0]
		std::string index = it->second.find("index")->second[0];
		index = index.substr(0, index.find(';'));

		// Open the file corresponding to the index
		std::ifstream file((this->_server->getRoot() + this->url + index).c_str());
		if (file.is_open()) {
			// Read the content of the file
			buffer << file.rdbuf();
			content = buffer.str();
			file.close();

			// Generate the HTTP 200 OK response with the content of the file
			generateResponse(oss, this->http_version, "200 OK", content);
		} else {
			// File not found, generate a 404 Not Found response
			generateErrorResponse(oss, 404);
		}
	} else {
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
	oss << http_version << " " << status_code << "\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << content.length() << "\r\n";
	oss << "\r\n";
	oss << content;
}

void Http::generateErrorResponse(std::ostringstream& oss, int error_code) {
	// Find the error page corresponding to the error code
	std::map<int, std::string>::const_iterator error_page_it;
	error_page_it = this->_server->getErrorMap().find(error_code);

	if (error_page_it != this->_server->getErrorMap().end()) {
		std::string path = this->_server->getErrorMap()[error_code];
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
