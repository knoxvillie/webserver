/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:27:31 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/25 14:52:53 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpServer.hpp"

TcpServer::TcpServer(const Config& object) : data(object) {
	GPS;
	 /*
	 * domain	: Specifies the communication domain. Protocol family that the socket will belong to.
	 * 			For a TCP/IP socket using the IPv4 Internet protocols defined by the AF_INET domain.
	 * type 	: Type of communication structure the socket will allow for this protocol family.
	 *			SOCK_STREAM - to allow for reliable, full-duplex byte streams.
	 * protocol	: Particular protocol the socket will use from the given family of protocols that
	 * 			support the chosen type of communication. For the AF_INET family, there is only
	 * 			one protocol that supports SOCK_STREAM. We will be setting this parameter to 0.
	 * backlog	: argument is the maximum number of connection threads we want to be able to hold at once.
	 * 			If a client tries to connect when the queue is full, they will get rejected by the server.
	 * */
	this->server_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (this->server_sock < 0)
		throw std::runtime_error("Error: Couldn't create socket");
	// The len parameter specifies the size of the address structure passed as the second argument (sockaddr* addr).
	if (bind(this->server_sock, (sockaddr *)(&this->data.server_address), sizeof(this->data.server_address)) < 0)
		throw std::runtime_error("Error: Couldn't bind socket");
	if (listen(this->server_sock, BACKLOG) < 0) //SOMAXCONN
		throw std::runtime_error("Error: Couldn't listen");
	//this->serverLoop();
}

TcpServer::~TcpServer() {
	GPS;
	//closeServer();
}

/* void
TcpServer::serverLoop(void) {
	GPS;
	int epoll_fd, num_ready_events, client_sock;
	struct epoll_event event, event_buffer[MAX_EVENTS];
	
	epoll_fd = epoll_create(1);
	
	if (epoll_fd < 0)
		throw std::runtime_error("Error: Creating epoll instance");
	event.events = EPOLLIN;
	event.data.fd = this->server_sock;
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, this->server_sock, &event) < 0)
		throw std::runtime_error("Error: epoll_ctl failed");
	
	while (true) {
		num_ready_events = epoll_wait(epoll_fd, event_buffer, MAX_EVENTS, -1);
		
		if (num_ready_events < 0) 
			break; // ????
		
		for (int i = 0; i < num_ready_events; i++) {
			if (event_buffer[i].data.fd == this->server_sock) {
				client_sock = this->acceptConnection();
				event_buffer[i].events = EPOLLIN;
				event_buffer[i].data.fd = client_sock;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event_buffer[i]) < 0)
					throw std::runtime_error("Error: epoll_ctl failed");
			} else {
				this->handleConnection(event_buffer[i].data.fd);
			}
		}
	}
	close (epoll_fd);
} */

int
TcpServer::acceptConnection(void) {
	int client_sock;
	struct sockaddr_in client_address;
	socklen_t client_address_len = sizeof(client_address);

	client_sock = accept(this->server_sock, (sockaddr *)&client_address, (socklen_t*)&client_address_len);
	return (client_sock < 0 ? throw std::runtime_error("Error: Client socket failed") : client_sock);
}

void
TcpServer::closeServer(void) {
	GPS;
	std::cout << "The server was closed" << std::endl;
	close(this->server_sock);
}

/*
	Example of the first request firefox does to a server:

	- ChatGPT:
		GET / HTTP/1.1
		Host: www.example.com
		User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:98.0) Gecko/20100101 Firefox/98.0
		Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,* / *;q=0.8
		Accept-Language: en-US,en;q=0.5
		Accept-Encoding: gzip, deflate, br
		Connection: keep-alive

	- Printing the buffer:
		GET / HTTP/1.1
		Host: 127.0.0.1:8080
		User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:123.0) Gecko/20100101 Firefox/123.0
		Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,* / *;q=0.8
		Accept-Language: en-US,en;q=0.5
		Accept-Encoding: gzip, deflate, br
		Connection: keep-alive
		Upgrade-Insecure-Requests: 1
		Sec-Fetch-Dest: document
		Sec-Fetch-Mode: navigate
		Sec-Fetch-Site: none
		Sec-Fetch-User: ?1
		DNT: 1
		Sec-GPC: 1
		
		 d*��

*/

void
TcpServer::handleConnection(int connection_socket) {
	GPS;

	// GET index.html HTTP/1.1
	// <command> <filename> HTTP/1.1
	/*
		- Parse the document
		- Open the file in the local system
		- Write the document back to the client
	*/
	char content[BUFFER_SIZE] = {0};

	if (recv(connection_socket, content, BUFFER_SIZE, 0) < 0)
		throw std::runtime_error("Error: Read from client socket");
	sendResponse(connection_socket);
}

void
TcpServer::parseRequest(int connection_socket, std::string& request) {
    // Check the command thats being used
    (void)connection_socket;
    (void)request;
}

std::string
TcpServer::buildResponse(void) {
	// The response should start with "HTTP/1.1 200 OK" if the resquest is successful.
	// Then there should be some more stuff, it could be a html file or something else

/*
	Basically, responses consist of the following elements:

	-The version of the HTTP protocol they follow.
	-A status code, indicating if the request was successful or not, and why.
	-A status message, a non-authoritative short description of the status code.
	-HTTP headers, like those for requests.
	-Optionally, a body containing the fetched resource.

	- List possible response codes and status messages;
	- Define an index.html;
	- Define function to handle the different types of requests that can be received = GET/POST/DELETE
	- List possible response types (cgi goes here, how to handle it?);
	- 
*/
	return (NULL); // temp
}

void
TcpServer::sendResponse(int connection_socket) {
	// We should use the send() function instead of write() as it gives us more
	// option on how to handle the content we send as well as being particularly 
	// useful for working with network sockets, such as those used in HTTP server development.

	// test sending something for now
	std::ostringstream oss;
	oss << "HTTP/1.1 200 OK\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: 15\r\n";
	oss << "\r\n";
	oss << "<h1>Hello</h1>";
	
	std::string output = oss.str();
	int size = output.size() + 1;

	send(connection_socket, output.c_str(), size, 0);
}

int
TcpServer::getSocket() {
	return (server_sock);
}
