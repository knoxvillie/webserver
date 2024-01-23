/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 10:00:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/01/15 10:00:15 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

Parser::Parser(const std::string &config_file) {
	std::ifstream inputFile(config_file.c_str());

	if (inputFile.is_open()) {
		std::string line;
		while (std::getline(inputFile, line)) {
			if ()
		}
	} else {
		std::cout << "Cannot open the config file" << std::endl;
		return (1);
	}

}