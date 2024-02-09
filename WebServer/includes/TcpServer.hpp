/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:24:39 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/09 12:31:59 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

class TcpServer {

    private:
        Config m_config;

        std::string m_ip_address;
        int m_port;
        int m_socket;
        struct sockaddr_in m_sockaddr;
        unsigned int m_sockaddr_len;
        
        //int m_clientSocket;
        //struct sockaddr_in m_clientAddress;

        int startServer(void);
        void closeServer(void);
        void startListen(void);
        void acceptConnection(void);

        std::string buildResponse(void);
        void sendResponse(void);

    public:
        TcpServer(const Config& config);
        ~TcpServer();
};
