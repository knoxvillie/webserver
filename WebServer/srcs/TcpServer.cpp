/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:27:31 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/19 16:32:43 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpServer.hpp"

TcpServer::TcpServer(const Server& config) : m_config(config) {
	struct sockaddr_in server_address;

	std::memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(this->m_config.s_port); //Why short?
	server_address.sin_addr.s_addr = inet_addr(this->m_config.s_host.c_str());
	this->m_servaddr = server_address;

    if (startServer() != 0) {
        MERROR("couldn't start server."); // maybe its better to throw an exception?
    }
}

TcpServer::~TcpServer() {
	closeServer();
}

int
TcpServer::startServer(void) {
	this->m_socket = socket(AF_INET, SOCK_STREAM, 0);

	if (this->m_socket < 0) {
		MERROR("couldn't create socket.");
	}
	if (bind(this->m_socket, (sockaddr *)(&this->m_servaddr), sizeof(this->m_servaddr)) < 0) {
		MERROR("couldn't bind socket.");
	}
	startListen();
	return (0);
}

void
TcpServer::closeServer(void) {
	close(this->m_socket);
	exit(0);
}

void
TcpServer::startListen(void) {
	if (listen(this->m_socket, 42) < 0) { // the max number of clients is just a placeholder
		MERROR("listen failed.");
	}
	serverLoop();
}

int
TcpServer::acceptConnection(void) {
	
	int conn_socket;
	socklen_t c_addr_len;
	struct sockaddr_in client_addr;
	
	if ((conn_socket = accept(m_socket, (sockaddr *)&client_addr, (socklen_t*)&c_addr_len)) < 0) {
		MERROR("connection failed.");
	}
	return conn_socket;
}

void
TcpServer::serverLoop(void) {

	GPS;
	this->epollfd = epoll_create1(0);
	if (this->epollfd < 0) {
		MERROR("epoll_create() failed.");
	}
	ev.events = EPOLLIN | EPOLLOUT; // ?
	ev.data.fd = m_socket;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, m_socket, &ev) < 0) {
		MERROR("epoll_ctl() failed.");
	}

	while (true) {
		std::cout << "Looking for connections on address " << inet_ntoa(this->m_servaddr.sin_addr) << ":" << m_config.s_port << " ... " <<std::endl;

		nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
		if (nfds < 0) {
			MERROR("epoll_wait() failed.");
		}

		for (int i = 0; i < nfds; ++i) {
			m_conn_socket = events[i].data.fd;

			if (m_conn_socket == m_socket) { // New connections
				m_conn_socket = acceptConnection();
				
				// maybe need to set the conn_socket to nonblocking ?
				
				struct epoll_event ev1;
				ev1.events = EPOLLIN | EPOLLOUT; // ?
				ev1.data.fd = m_conn_socket;
				
				epoll_ctl(epollfd, EPOLL_CTL_ADD, m_conn_socket, &ev1);
			}
			else if (events[i].events == EPOLLHUP) {
				close(m_conn_socket); // might need to call epoll_ctl + EPOLL_CTL_DEL
			}
			else {
				handleConnection(m_conn_socket); // might need to send the event and not only the fd
			}
		}
	}
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

    // Change this to max client bodysize ???
    std::string buf1;
    buf1.resize(BUFFER_SIZE);

    char buf[BUFFER_SIZE];

	int bytesIn = recv(connection_socket, &buf, BUFFER_SIZE, 0);  

    if (bytesIn <= 0) {
        // drop client ?
    }
    else {
        MLOG(buf);
        //parseRequest(connection_socket, buf);
    }

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
	oss << "Content-Type: text/plain\r\n";
	oss << "Content-Length: 5\r\n";
	oss << "\r\n";
	oss << "Hello";
	
	std::string output = oss.str();
	int size = output.size() + 1;

	send(connection_socket, output.c_str(), size, 0);
}
