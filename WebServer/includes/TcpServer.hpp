/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:24:39 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/29 17:03:03 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "ServerConfig.hpp"

/*
HTTP Server: 
    An HTTP server is a specific type of TCP server that is designed to handle Hypertext Transfer Protocol (HTTP) requests.
    HTTP is an application layer protocol that is used for retrieving web pages, data, and resources over the World Wide Web.
    An HTTP server listens on a specific TCP port (typically port 80 for HTTP and port 443 for secure HTTPS) and responds to
    HTTP requests from clients, such as web browsers. 
    It processes these requests, retrieves web content, and sends it back to the clients.
*/

class Server;

class TcpServer {

    private:
        TcpServer(); // Prevent the use of the default constructor

        ServerConfig m_config;

        std::string m_ip_address;
        int m_port, m_socket;
        struct sockaddr_in m_servaddr;
        unsigned int m_servaddr_len;

        fd_set current_sockets, ready_sockets;

        int startServer(void);
        void closeServer(void);
        void startListen(void);
        int acceptConnection(void);

        void serverLoop(void);
        void handleConnection(int connection_socket);

        std::string buildResponse(void);
        void sendResponse(void);

    public:
        TcpServer(const ServerConfig& config);
        ~TcpServer();
};
