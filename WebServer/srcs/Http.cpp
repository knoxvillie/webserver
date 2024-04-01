/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 18:34:53 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/01 18:34:53 by kfaustin         ###   ########.fr       */
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
    Host: Specifies the host and port to which the request is being sent. In this case, it's "0.0.0.0:8080".
    User-Agent: Provides information about the client making the request. In this case, it indicates that the user agent is Mozilla Firefox running on Ubuntu Linux.
    Accept: Indicates what media types the client can understand. It prefers HTML but also accepts other formats like XML and images.
    Accept-Language: Specifies the preferred language for the response. In this case, it prefers English.
    Accept-Encoding: Indicates the types of content encodings that the client supports. Here, it supports gzip and deflate compression methods.
    Connection: Specifies whether the connection should be kept alive after the current request/response cycle.
    Upgrade-Insecure-Requests: Indicates that the client would like the server to upgrade the request to a secure HTTPS connection if possible.*/


Http::Http(int connection, Server* server) : _client(connection), _server(server) {
	this->requestFromClient();
	this->requestParser();
	this->responseSend();
}

void Http::requestFromClient() {
	GPS;
	char content[BUFFER_SIZE] = {0};

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

/*void
Http::responseSend(void) {
	GPS;
	std::ostringstream oss;
	std::map<std::string, std::map<std::string, std::vector<std::string> > >& location = this->_server->getLocation();
	std::map<std::string, std::map<std::string, std::vector<std::string> > >::const_iterator it = location.begin();

	(void)location;
	(void)it;
	if (this->url == "/") {
		std::string content;
		std::ifstream file(std::string(this->_server->getRoot() + "/" + this->_server->getIndex()).c_str());

		std::cout << std::string(this->_server->getRoot() + "/" + this->_server->getIndex()) << std::endl;
		if (file.is_open()) {
			std::stringstream buffer;

			buffer << file.rdbuf();
			content = buffer.str();

			oss << this->http_version << " 200 OK\r\n";
			oss << "Cache-Control: no-cache, private\r\n";
			oss << "Content-Type: text/html\r\n";
			oss << "Content-Length: " << content.length() + 2 << "\r\n";
			oss << "\r\n"; // Terminate headers properly
			oss << content; // Include file content in the response body
			file.close();
		} else throw std::runtime_error("Error: Invalid content to serve");
	}
	std::string response(oss.str());
	std::cout << response << std::endl;

	//MSG_DONTWAIT flag can cause the send function to return immediately, possibly before the data is actually sent.
	//This can result in incomplete responses. Consider using a blocking send for simplicity, or properly handling
	//non-blocking sends with error checking and handling retries if necessary.
	if (send(this->_client, response.c_str(), response.length() + 1, MSG_DONTWAIT) < 0)
		throw std::runtime_error("Error: send function failed");
}*/

/*
GET /lacrimosa.mp3 HTTP/1.1
Host: 127.0.0.1:8080
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:124.0) Gecko/20100101 Firefox/124.0
Accept: audio/webm,audio/ogg,audio/wav,audio*/
/*;q=0.9,application/ogg;q=0.7,video/*;q=0.6,*//*
*;q=0.5
Accept-Language: en-US,en;q=0.5
Range: bytes=0-
			 Connection: keep-alive
		Referer: http://127.0.0.1:8080/
Cookie: csrftoken=v2M9Xz9Js2eQqZyi6nzAWVDaoIYuHNOc; username-127-0-0-1-8888="2|1:0|10:1711492418|23:username-127-0-0-1-8888|44:ZDBjOGU0NWFhM2Q0NGM2OGFjOGQ0NTRhOGQwMzhjZjA=|701de7e960daea0b2eb837062378c34a3f11abb2c6b8042089ac3288d4d8fb63"
Sec-Fetch-Dest: audio
		Sec-Fetch-Mode: no-cors
		Sec-Fetch-Site: same-origin
		Accept-Encoding: identity

*/


void Http::responseSend(void) {
	std::ostringstream oss;
	if (this->url == "/") {
		std::ifstream file(std::string(this->_server->getRoot() + "/" + this->_server->getIndex()).c_str());
		if (file.is_open()) {
			std::stringstream buffer;
			buffer << file.rdbuf();
			std::string content = buffer.str();

			oss << this->http_version << " 200 OK\r\n";
			oss << "Cache-Control: no-cache, private\r\n";
			oss << "Content-Type: text/html\r\n";
			oss << "Content-Length: " << content.length() << "\r\n";
			oss << "\r\n";
			oss << content;

			file.close();
		} else {
			throw std::runtime_error("Error: Unable to open file");
		}
	}
	std::string response = oss.str();
	std::cout << response << std::endl;

	/*
	 * MSG_DONTWAIT flag can cause the send function to return immediately, possibly before the data is actually sent.
	 * 	This can result in incomplete responses. Consider using a blocking send for simplicity, or properly handling
	 * 	non-blocking sends with error checking and handling retries if necessary.
	*/
	 // Use blocking send for simplicity
	if (send(this->_client, response.c_str(), response.length(), 0) < 0) {
		throw std::runtime_error("Error: send function failed");
	}
}


Http::~Http() {}
