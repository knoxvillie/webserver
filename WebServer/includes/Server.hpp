/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 16:27:55 by kfaustin          #+#    #+#             */
/*   Updated: 2024/03/12 15:50:44 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"
#include "Parser.hpp"


/*
	This class should contain all basic configuration needed to start a server,
	the point is that a TcpServer object can be created by passing an instance of
	this class.
*/
class Server {
	
	private:
		Server(void); // Preventing the compiler from creating a default constructor.

		std::map<std::string, std::vector<std::string> > _serverDirectives;
		std::map<std::string, std::map<std::string, std::vector<std::string> > > _locationDirectives;

		// Variables to store data from the config file
		struct in_addr ipAddress;

		// Listen
		std::string s_host;
		uint16_t s_port;
		// Server_name
		std::string server_name;
		// Root
		std::string root;
		// Index
		std::string index;
		// Auto Index
		bool auto_index;
		// Allow Methods
		std::vector<std::string> allow_methods;
		// Client Max Body Size
		uint16_t cMaxBodySize;




		//        std::string m_server_name;          // Server domain name
//        std::string m_host;                 // IP
//        std::string m_root;                 // Root repository for the server files
//        std::string m_index;                // Index page
//        std::string m_charset;              // charset to be used, eg. uft-8
//        std::string m_access_log;           // Where to log accesses
//        std::string m_error_log;            // Where to log errors
//        std::string m_error_page;           // Default error page
//        std::string m_location;             // Where to look for requested files
//        std::string m_clien_max_body_size; // Max allowed size of a client request body

	public:
		Server(std::map<std::string, std::vector<std::string> >&, std::map<std::string, std::map<std::string, std::vector<std::string> > >&);
		std::map<std::string, std::vector<std::string> > &getServerDirectives(void);
		std::map<std::string, std::map<std::string, std::vector<std::string> > > &getLocationDirectives(void);
		void applyServerDirectives(void);
		void validateServerDirectives(void);

		//Directives parser
		void checkListen(std::vector<std::string>&);
		void checkServerName(std::vector<std::string>&);
		void checkRoot(std::vector<std::string>&);
		void checkIndex(std::vector<std::string>&);
		void checkAutoIndex(std::vector<std::string>&);
		void checkAllowMethods(std::vector<std::string>&);
		void checkClientMaxBodySize(std::vector<std::string>&);
		void checkErrorPage(std::vector<std::string>&);
};

#endif //SERVER_HPP
