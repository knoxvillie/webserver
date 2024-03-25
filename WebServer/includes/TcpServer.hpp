/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:24:39 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/25 13:46:03 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Config.hpp"

/*
HTTP Server: 
	An HTTP server is a specific type of TCP server that is designed to handle Hypertext Transfer Protocol (HTTP) requests.
	HTTP is an application layer protocol that is used for retrieving web pages, data, and resources over the World Wide Web.
	An HTTP server listens on a specific TCP port (typically port 80 for HTTP and port 443 for secure HTTPS) and responds to
	HTTP requests from clients, such as web browsers. 
	It processes these requests, retrieves web content, and sends it back to the clients.
*/

// ========================
//     TcpServer Class
// ========================

#define BACKLOG 42
#define MAX_EVENTS 10
#define BUFFER_SIZE 4096 // Change this ??

class TcpServer {

	private:
		friend class Cluster;

		TcpServer(); // Prevent the use of the default constructor

		Config data;
		int server_sock;

		int fd_epoll;
		int nfds;
		struct epoll_event ev, events[MAX_EVENTS];

		void startServer(void);
		void startListen(unsigned short);
		void closeServer(void);
		int acceptConnection(void);

		void serverLoop(void);
		void handleConnection(int);
		void parseRequest(int connection_socket, std::string& request);

		std::string buildResponse(void);
		void sendResponse(int connection_socket);

	public:
		TcpServer(const Config& config);
		~TcpServer();

		int getSocket();
};
