/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 12:43:34 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/24 12:43:37 by kfaustin         ###   ########.fr       */
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
		static void parsingConfigFile(const std::string&);;
		static std::vector<Server> &getServers(void);
};
