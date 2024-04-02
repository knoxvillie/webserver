/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 09:36:33 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/02 15:59:35 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Parser.hpp"
#include "Cluster.hpp"

int	main(int argc, char* argv[], char* env[]) {
	GPS;
	
	if (argc > 2) {
		MERROR("wrong number of arguments.\nTry: \"./webserv [configuration file]\"");
	}
	signal(SIGINT, signal_handler);
	const std::string config_file(argc == 1 ? "./conf/default.conf" : argv[1]);

	try {
		Parser::setPWD(env);
		Parser::parsingConfigFile(config_file);
		Cluster::startServers(Parser::getServers());
	}
	catch (const std::runtime_error& except) {
		std::cout << std::endl;
		std::cerr << except.what();
		std::cout << std::endl;
	}
	return (0);
}
