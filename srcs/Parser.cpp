/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 10:00:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/01/23 15:07:08 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"

Parser::Parser(const std::string& config_file) {
	// Passing a empty string in ifstream parameter will result in undefined behaviour.
	if (config_file.empty()) {
		std::cout << "The config file cannot be empty" << std::end;
		return (1);
	}
	std::ifstream inputFile(config_file.c_str());
	if (inputFile.is_open()) {
		// Peek looks at the next character in the stream. If peek returns EOF the file is empty.
		if (inputFile.peek() == std::ifstream::traits_type::eof()) {
			std::cout << "The config file hasn't content" << std::endl;
			return (1);
		}
		std::string line;
		while (std::getline(inputFile, line)) {
			std::stringstream ss(line);
			std::string token;
			ss >> token;
			if (token == "server") {
				
			}
		}
	}
	} else {
		std::cout << "Cannot open the config file" << std::endl;
		return (1);
	}

}