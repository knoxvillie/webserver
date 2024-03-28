/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/12 16:27:55 by kfaustin          #+#    #+#             */
/*   Updated: 2024/03/27 18:52:41 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef CONFIG_HPP
# define CONFIG_HPP

#include "webserv.hpp"
#include "Parser.hpp"


class Config {
	
	friend class TcpServer; // so TcpServer class can access private members

	private:
		Config(void); // Preventing the compiler from creating a default constructor.

		std::map<std::string, std::vector<std::string> > _serverDirectives;
		std::map<std::string, std::map<std::string, std::vector<std::string> > > _locationDirectives;

		int server_sock;
		struct sockaddr_in server_address;
		std::string s_host;
		uint16_t s_port;
		std::string server_name;
		std::string root;
		std::string index;
		bool auto_index;
		std::vector<std::string> allow_methods;
		uint16_t cMaxBodySize;
		std::map<int, std::vector<std::string> > error_page;

	public:
		~Config(void);
		Config(std::map<std::string, std::vector<std::string> >&, std::map<std::string, std::map<std::string, std::vector<std::string> > >&);
		std::map<std::string, std::vector<std::string> > &getServerDirectives(void);
		std::map<std::string, std::map<std::string, std::vector<std::string> > > &getLocationDirectives(void);
		void applyServerDirectives(void);
		void validateServerDirectives(void);
		void startServerSocket(void);
		int acceptConnection(void);

		//Directives parser
		void directiveSelector(const std::string&, std::vector<std::string>&);
		void checkListen(std::vector<std::string>&);
		void checkServerName(std::vector<std::string>&);
		void checkRoot(std::vector<std::string>&);
		void checkIndex(std::vector<std::string>&);
		void checkAutoIndex(std::vector<std::string>&);
		void checkAllowMethods(std::vector<std::string>&);
		void checkClientMaxBodySize(std::vector<std::string>&);
		void checkErrorPage(std::vector<std::string>&);
};

#endif //CONFIG_HPP
