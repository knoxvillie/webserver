/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 16:27:55 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/05 12:56:51 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef SERVER_HPP
# define SERVER_HPP

#include "webserv.hpp"
#include "Parser.hpp"



class Server {
	private:
		Server(void); // Preventing the compiler from creating a default constructor.

		std::map<std::string, std::vector<std::string> > _serverDirectives;
		std::map<std::string, std::map<std::string, std::vector<std::string> > > _locationDirectives;

		// Global config
		std::string _pwd;

		//	Server
		int server_sock;
		struct sockaddr_in server_address;

		// Server config
		uint16_t s_port;
		std::string s_host;
		std::string server_name;
		std::map<int, std::string> error_page;

		// Locations
		std::vector<t_location> locations;

	public:
		~Server(void);
		Server(std::map<std::string, std::vector<std::string> >&, std::map<std::string, std::map<std::string, std::vector<std::string> > >&, std::string&);

		//	Methods
		void applyServerDirectives(void);
		void validateServerDirectives(void);
		void startServerSocket(void);
		int acceptConnection(void) const;

		//	Getters
		std::map<std::string, std::vector<std::string> > &getServer(void);
		std::map<std::string, std::map<std::string, std::vector<std::string> > > &getLocationMap(void);
		int getSocket(void) const;
		std::string getPWD(void) const;
		std::map<int, std::string> getErrorMap(void) const;
		t_location* getLocation(const std::string&);
		void * getDirectiveFromLocation(std::vector<t_location>&, const std::string&, const std::string&);

		//Server Directives Parser
		void checkListen(std::vector<std::string>&);
		void checkServerName(std::vector<std::string>&);
		void checkErrorPage(std::vector<std::string>&);

		//Location Directives Parser
		void checkRoot(std::vector<std::string>&, t_location&);
		void checkIndex(std::vector<std::string>&, t_location&);
		void checkAutoIndex(std::vector<std::string>&, t_location&);
		void checkClientMaxBodySize(std::vector<std::string>&, t_location&);
		void checkAllowMethods(std::vector<std::string>&, t_location&);
		void checkCgi(std::vector<std::string>&, t_location&);
		void checkRedirect(std::vector<std::string>&, t_location&);
};

#endif //SERVER_HPP
