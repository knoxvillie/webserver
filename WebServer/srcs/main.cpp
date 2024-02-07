/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 09:36:33 by kfaustin          #+#    #+#             */
/*   Updated: 2024/02/07 12:37:21 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Parser.hpp"

int	main(int argc, char* argv[]) {
	if (argc > 2) {
		std::cout << "Webserver must have only 1 argument" << std::endl;
		return (1);
	}
	const std::string config_file(argc == 1 ? "../conf/default.conf" : argv[1]);
	MLOG("Config file path", config_file);
	try {
		Parser::parsingConfigFile(config_file);
	} catch (const std::runtime_error& except) {
		std::cerr << except.what() << std::endl;
	}
	return (0);
}