/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/07 12:24:39 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/18 15:33:49 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Server.hpp"

/*
HTTP Server: 
    An HTTP server is a specific type of TCP server that is designed to handle Hypertext Transfer Protocol (HTTP) requests.
    HTTP is an application layer protocol that is used for retrieving web pages, data, and resources over the World Wide Web.
    An HTTP server listens on a specific TCP port (typically port 80 for HTTP and port 443 for secure HTTPS) and responds to
    HTTP requests from clients, such as web browsers. 
    It processes these requests, retrieves web content, and sends it back to the clients.
*/

// ========================
//      Config Struct	
// ========================

typedef struct s_config
{
    // Variables to store data from the config file
	/*
		NOTE: MOST VARIABLES ARE STRINGS FOR NO REASON, FEEL FREE TO CHANGE IF NEEDED
	*/
    std::vector<std::string> listen;               // Will give us the port (and protocols ??)
    std::vector<std::string> server_name;          // Server domain name
    std::vector<std::string> host;                 // IP
    std::vector<std::string> root;                 // Root repository for the server files
    std::vector<std::string> index;                // Index page
    std::vector<std::string> charset;              // charset to be used, eg. uft-8
    std::vector<std::string> access_log;           // Where to log accesses
    std::vector<std::string> error_log;            // Where to log errors
    std::vector<std::string> error_page;           // Default error page
    std::vector<std::string> client_max_body_size; // Max allowed size of a client request body
	// Location: Where to look for requested files
    std::map<std::string, std::map<std::string, std::vector<std::string> > > location;
	// map< URI, map< Directive, values > >

}   ServerConfig;


// ========================
//     TcpServer Class
// ========================

#define MAX_EVENTS 10

class TcpServer {

    private:
        TcpServer(); // Prevent the use of the default constructor

        Server m_config;

        std::string m_ip_address;
        uint16_t m_port;
        int m_socket, m_conn_socket;
        struct sockaddr_in m_servaddr;
        unsigned int m_servaddr_len;
        
        int epollfd, nfds;
        struct epoll_event ev, events[MAX_EVENTS];

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
        TcpServer(const Server& config);
        ~TcpServer();
};
