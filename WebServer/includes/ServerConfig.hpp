/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 11:52:38 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/29 15:32:27 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

/*
	This class should contain all basic configuration needed to start a server,
	the point is that a TcpServer object can be created by passing an instance of
	this class.
*/

class ServerConfig {
	
	friend class TcpServer; // so TcpServer class can access private members

	private:
		// Variables to store data from the config file
		/*
			NOTE: MOST VARIABLES ARE STRINGS FOR NO REASON, FEEL FREE TO CHANGE IF NEEDED
		*/
        std::vector<std::string> m_listen;               // Will give us the port (and protocols ??)
        std::vector<std::string> m_server_name;          // Server domain name
        std::vector<std::string> m_host;                 // IP
        std::vector<std::string> m_root;                 // Root repository for the server files
        std::vector<std::string> m_index;                // Index page
        std::vector<std::string> m_charset;              // charset to be used, eg. uft-8
        std::vector<std::string> m_access_log;           // Where to log accesses
        std::vector<std::string> m_error_log;            // Where to log errors
        std::vector<std::string> m_error_page;           // Default error page
        std::vector<std::string> m_client_max_body_size; // Max allowed size of a client request body
		// Location: Where to look for requested files
        std::map<std::string, std::map<std::string, std::vector<std::string> > > m_location;
		// map< URI, map< Directive, values > >

        void setConfigFileVariables(const std::map<std::string, std::vector<std::string> >& directives);
		bool changeVariable(const std::string &name, const std::vector<std::string>& value);

	public:
		ServerConfig(); // Should either be private or initiate with default values
		ServerConfig(std::map<std::string, std::vector<std::string> >& server, std::map<std::string, std::map<std::string, std::vector<std::string> > >& location);
		ServerConfig(const ServerConfig& obj);
		ServerConfig& operator=(const ServerConfig& obj);
		~ServerConfig();

		// Getter functions
		std::string getListenPort() const;
		std::string getHostIP() const;
};
