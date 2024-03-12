/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:27:31 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/12 14:01:17 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpServer.hpp"

TcpServer::TcpServer(const ServerConfig& config) : 
m_config(config), m_servaddr(), m_servaddr_len(sizeof(m_servaddr))
{
    FD_ZERO(&current_sockets);

    m_ip_address = "127.0.0.1"; //m_config.host[1];      index???????????????
    m_port = 80;                //atoi((m_config.listen[1]).c_str());       index???????????????

    m_servaddr.sin_family = AF_INET;
    m_servaddr.sin_port = htons(m_port);
    m_servaddr.sin_addr.s_addr = inet_addr(m_ip_address.c_str());

    if (startServer() != 0) {
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

    if (bind(m_socket, (sockaddr *)&m_servaddr, m_servaddr_len) < 0) {
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
    
    if (listen(m_socket, 42) < 0) { // the max number of clients is just a placeholder
        MERROR("listen failed.");
    }
    // look for connections and accept them
    serverLoop();
}

int
TcpServer::acceptConnection(void) {
    
    int conn_socket;
    socklen_t c_addr_len;
    struct sockaddr_in client_addr;
    
    if (conn_socket = accept(m_socket, (sockaddr *)&client_addr, (socklen_t*)&c_addr_len) < 0) {
        MERROR("connection failed.");
    }
    return conn_socket;
}

void
TcpServer::serverLoop(void) {

    epollfd = epoll_create1(0);
    if (epollfd < 0) {
        MERROR("epoll_create() failed.");
    }

    ev.events = EPOLLIN; // EPOLLOUT ?
    ev.data.fd = m_socket;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, m_socket, &ev) < 0) {
        MERROR("epoll_ctl() failed.");
    }

    while (true) {
        std::cout << "Looking for connections on address " << inet_ntoa(this->m_servaddr.sin_addr) << "... " <<std::endl;
        
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            MERROR("epoll_wait() failed.");
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == m_socket) { // New connections
                m_conn_socket = acceptConnection();
            }
        }

    }
}

/* void
TcpServer::serverLoop(void) {

    FD_SET(m_socket, &current_sockets);

    while (true) {
        std::cout << "Looking for connections on address " << inet_ntoa(this->m_servaddr.sin_addr) << "... " <<std::endl;
        
        // Because select() is destructive
        ready_sockets = current_sockets;

        // max nbr of FDs in a set, read FD set, write FD set, error FD set, timeout value  
        if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0) {
            MERROR("select failed.");
        }

        for (int i = 0; i < FD_SETSIZE; i++) {
            if (FD_ISSET(i, &ready_sockets)) {
                if (i == m_socket) {
                    // This means its a new connection
                    m_conn_socket = acceptConnection();
                    FD_SET(m_conn_socket, &current_sockets);
                } 
                else {
                    handleConnection(i);
                    FD_CLR(i, &current_sockets); // This should be done after closing the socket
                }
            }
        }
    }
} */

void
TcpServer::handleConnection(int connection_socket) {
    
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
