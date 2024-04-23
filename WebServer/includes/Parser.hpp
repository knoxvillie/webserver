/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 10:00:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/16 15:28:33 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class Server;

class Parser {
	private:
		Parser(void);
		~Parser(void);
		static std::vector<Server> _servers;
		static std::map<std::string, std::vector<std::string> >	_directives;
		static std::map<std::string, std::map<std::string, std::vector<std::string> > > _locations;

	public:
		static const char* server_directives[];
		static const char* location_directives[];
		static void parsingConfigFile(const std::string&);
		static void parsingDirectives(const std::string&, std::vector<std::string>&, std::map<std::string, std::vector<std::string > >&);
		static void parsingLocationBlock(std::vector<std::string>&);
		static std::vector<Server> &getServers(void);
};