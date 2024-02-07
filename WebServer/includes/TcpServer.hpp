/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:24:39 by diogmart          #+#    #+#             */
/*   Updated: 2023/11/14 14:09:51 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

/*
HTTP Server: 
    An HTTP server is a specific type of TCP server that is designed to handle Hypertext Transfer Protocol (HTTP) requests.
    HTTP is an application layer protocol that is used for retrieving web pages, data, and resources over the World Wide Web.
    An HTTP server listens on a specific TCP port (typically port 80 for HTTP and port 443 for secure HTTPS) and responds to
    HTTP requests from clients, such as web browsers. 
    It processes these requests, retrieves web content, and sends it back to the clients.
*/

#define PORT 80 // typical port for an HTTP server

class TcpServer {

    public:
        TcpServer(std::string ip_address, int port);
        ~TcpServer();
    
    private:
        std::string m_ip_address;
        int m_port;
        int m_socket;
        struct sockaddr_in m_sockaddr;
        unsigned int m_sockaddr_len;
        
        //int m_clientSocket;
        //struct sockaddr_in m_clientAddress;

        int setupServer(void);
        void closeServer(void);
        void startListen(void);
        void acceptConnection(void);

        std::string buildResponse(void);
        void sendResponse(void);
};
