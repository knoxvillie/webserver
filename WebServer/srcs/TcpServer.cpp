/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:27:31 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/29 14:58:55 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpServer.hpp"

TcpServer::TcpServer(const ServerConfig& config) : 
m_config(config), m_sockaddr(), m_sockaddr_len(sizeof(m_sockaddr))
{
    m_ip_address = m_config.getHostIP();
    m_port = atoi((m_config.getListenPort()).c_str());

    m_sockaddr.sin_family = AF_INET;
    m_sockaddr.sin_port = htons(m_port);
    m_sockaddr.sin_addr.s_addr = inet_addr(m_ip_address.c_str());

    if (startServer() != 0) { // omitted this->
        MERROR("couldn't start server."); // maybe its better to throw an exception to avoid leaks?
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

    if (bind(m_socket, (sockaddr *)&m_sockaddr, m_sockaddr_len) < 0) {
        MERROR("couldn't bind socket.");
    }

    return (0);
}

void
TcpServer::closeServer(void) {
    close(this->m_socket);
    exit(0);
}

void
TcpServer::startListen(void) {
    
    if (listen(m_socket, 42) < 0) { // the max number of clients is just a placeholder
        MERROR("socket listen failed.")
    }

    // look for connections and accept them
}

void
TcpServer::acceptConnection(void) {
    // We need to accept the connection, store the new socket fd in a variable
    // and the new socket address in another struct ?
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
*/
    return (NULL); // temp
}

void
TcpServer::sendResponse(void) {
    // We should use the send() function instead of write() as it gives us more
    // option on how to handle the content we send as well as being particularly 
    // useful for working with network sockets, such as those used in HTTP server development.
}
