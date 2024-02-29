/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 09:36:33 by kfaustin          #+#    #+#             */
/*   Updated: 2024/02/29 16:29:07 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Parser.hpp"
#include "TcpServer.hpp"
#include "Server.hpp"

int	main(int argc, char* argv[]) {
	GPS;
	if (argc > 2) {
		MERROR("wrong number of arguments.\nTry: \"./webserv [configuration file]\"");
	}
	const std::string config_file(argc == 1 ? "../conf/default.conf" : argv[1]);
	try {
		Parser::parsingConfigFile(config_file);
	}
	catch (const std::runtime_error& except) {
		std::cerr << except.what() << std::endl;
	}
	return (0);
}