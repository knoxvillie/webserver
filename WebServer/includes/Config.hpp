/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 11:52:38 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/09 12:26:05 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

/*
	This class should contain all basic configuration needed to start a server,
	the point is that a TcpServer object can be created by passing an instance of
	this class.
*/

class Config {
	
	private:
		// The vector needs to be private so it won't be altered outside of the class,
		// can't be const because of initialization
		static std::vector<std::string> directives;  // Init funtion called in main

		// Variables to store data from the config file
        std::string m_listen;               // Will give us the port (and protocols ??)
        std::string m_server_name;          // Server domain name
        std::string m_host;                 // IP
        std::string m_root;                 // Root repository for the server files
        std::string m_index;                // Index page
        std::string m_charset;              // charset to be used, eg. uft-8
        std::string m_access_log;           // Where to log accesses
        std::string m_error_log;            // Where to log errors
        std::string m_error_page;           // Default error page
        std::string m_location;             // Where to look for requested files
        std::string m_client_max_body_size; // Max allowed size of a client request body

        void setConfigFileVariables(const std::map<std::string, std::vector<std::string> > &directives);

	public:
		Config();
		Config(const Config& obj);
		~Config();
		
		static void initDirectivesVec();
		static std::vector<std::string> getDirectives(); 
};
