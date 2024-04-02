/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 16:27:55 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/02 15:48:43 by kfaustin         ###   ########.fr       */
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

		//	Server
		int server_sock;
		struct sockaddr_in server_address;

		// Server config
		uint16_t s_port;
		std::string s_host;
		std::string server_name;
		std::map<int, std::string> error_page;

		// Default location root
		std::string root;
		std::string index;
		bool auto_index;
		std::vector<std::string> allow_methods;
		uint16_t lCMaxBodySize;


	public:
		~Server(void);
		Server(std::map<std::string, std::vector<std::string> >&, std::map<std::string, std::map<std::string, std::vector<std::string> > >&);

		//	Methods
		void applyServerDirectives(void);
		void validateServerDirectives(void);
		void startServerSocket(void);
		int acceptConnection(void) const;

		//	Getters
		std::map<std::string, std::vector<std::string> > &getServer(void);
		std::map<std::string, std::map<std::string, std::vector<std::string> > > &getLocation(void);
		int getSocket(void) const;
		std::string getRoot(void) const;
		std::string getIndex(void) const;
		std::map<int, std::string> getErrorMap(void) const;

		//Directives parser
		void directiveSelector(const std::string&, std::vector<std::string>&, bool);
		void checkListen(std::vector<std::string>&);
		void checkServerName(std::vector<std::string>&);
		void checkRoot(std::vector<std::string>&, bool);
		void checkIndex(std::vector<std::string>&, bool);
		void checkAutoIndex(std::vector<std::string>&, bool);
		void checkAllowMethods(std::vector<std::string>&, bool);
		void checkClientMaxBodySize(std::vector<std::string>&);
		void checkErrorPage(std::vector<std::string>&);
};

#endif //SERVER_HPP
