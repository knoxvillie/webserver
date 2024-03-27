/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:27:31 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/27 16:05:21 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpServer.hpp"
#include "HttpRequest.hpp"

TcpServer::TcpServer(const Config& object) : data(object) {
	GPS;
	this->server_sock = socket(AF_INET, SOCK_STREAM, 0);

	if (this->server_sock < 0)
		throw std::runtime_error("Error: Couldn't create socket");
	// The len parameter specifies the size of the address structure passed as the second argument (sockaddr* addr).
	if (bind(this->server_sock, (sockaddr *)(&this->data.server_address), sizeof(this->data.server_address)) < 0) {
		std::cerr << "Bind failed: " << strerror(errno) << std::endl;
		throw std::runtime_error("Error: Couldn't bind socket");
	}
	if (listen(this->server_sock, SOMAXCONN) < 0)
		throw std::runtime_error("Error: Couldn't listen");
}

TcpServer::~TcpServer() {
	GPS;
	//closeServer();
}

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

	send(connection_socket, output.c_str(), size, MSG_DONTWAIT);
}

int
TcpServer::getSocket() {
	return (server_sock);
}
