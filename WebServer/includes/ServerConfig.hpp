/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 11:52:38 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/29 14:05:41 by diogmart         ###   ########.fr       */
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
	
	private:
		// The vector needs to be private so it won't be altered outside of the class,
		// can't be const because of initialization
		static std::vector<std::string> directives;  // Init funtion called in main

		// Variables to store data from the config file
		/*
			NOTE: MOST VARIABLES ARE STRINGS FOR NO REASON, FEEL FREE TO CHANGE IF NEEDED
				  (Would also need to change copy constructor and etc)
		*/
        std::string m_listen;               // Will give us the port (and protocols ??)
        std::string m_server_name;          // Server domain name
        std::string m_host;                 // IP
        std::string m_root;                 // Root repository for the server files
        std::string m_index;                // Index page
        std::string m_charset;              // charset to be used, eg. uft-8
        std::string m_access_log;           // Where to log accesses
        std::string m_error_log;            // Where to log errors
        std::string m_error_page;           // Default error page
        std::string m_client_max_body_size; // Max allowed size of a client request body
		// Location: Where to look for requested files
        std::map<std::string, std::map<std::string, std::vector<std::string> > > m_location;
		// map< URI, map< Directive, values > >

        void setConfigFileVariables(const std::map<std::string, std::vector<std::string> >& directives);
		bool changeVariable(const std::string &name, const std::string& value);

	public:
		ServerConfig(); // Should either be private or initiate with default values
		ServerConfig(const ServerConfig& obj);
		ServerConfig& operator=(const ServerConfig& obj);
		~ServerConfig();

		// Getter functions
		std::string getListen() const;
		std::string getServerName() const;
		std::string getHost() const;
		std::string getRoot() const;
		std::string getIndex() const;
		std::string getCharset() const;
		std::string getAccessLog() const;
		std::string getErrorLog() const;
		std::string getErrorPage() const;
		std::string getClientMaxBodySize() const;
		std::string getLocation() const;

		// Setter functions
/* 		void setListen(const std::string& listen);
		void setServerName(const std::string& serverName);
		void setHost(const std::string& host);
		void setRoot(const std::string& root);
		void setIndex(const std::string& index);
		void setCharset(const std::string& charset);
		void setAccessLog(const std::string& accessLog);
		void setErrorLog(const std::string& errorLog);
		void setErrorPage(const std::string& errorPage);
		void setClientMaxBodySize(const std::string& clientMaxBodySize);
		void setLocation(const std::string& location);	 */
		
		static void initDirectivesVec();
		static std::vector<std::string> getDirectives(); 
};
