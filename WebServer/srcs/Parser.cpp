/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 10:00:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/02/07 16:07:26 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(void) {}

void
Parser::parsingConfigFile(const std::string &config_file) {
	// Passing an empty string in ifstream parameter will result in undefined behaviour.
	if (config_file.empty())
		throw std::runtime_error("The config file cannot be empty");
	std::ifstream inputFile(config_file.c_str());

	if (inputFile.is_open()) {
		// Peek looks at the next character in the stream. If peek returns EOF the file is empty.
		if (inputFile.peek() == std::ifstream::traits_type::eof())
			throw std::runtime_error("The config file doesn't have content");	
		std::string line;
		std::string token;

		while (std::getline(inputFile, line)) {
			std::stringstream ss(line); ss >> token;
			if (token.empty() || token[0] == '#')
				continue; // It means that line is empty or a comment
			if (token != "server")
				throw std::runtime_error("Invalid block");
			
			ss >> token;
			if (token.empty() || token[0] != '{')
				throw std::runtime_error("Server block must be opened with `{");
			
			while (std::getline(inputFile, line)) {
				std::stringstream ss(line); ss >> token;
				if (token == "}") // Server block closing
					break;
				if (token.empty() || token[0] == '#')
					continue;
				if (!isTokenInDirectives(token)) // missing location block
					throw std::runtime_error("Invalid server directive");
				std::vector<std::string> vec(splitString(line));
				Parser::parsingDirectives(token, vec);
			}
		}
	} else
		throw std::runtime_error("Cannot open the config file");
	// Close inputFile
}

void
Parser::parsingDirectives(const std::string& directive, std::vector<std::string>& split) {
	if (split.empty())
		throw std::runtime_error(directive + " doesn't have values");
	std::vector<std::string>::iterator str = split.end();
	str--;
	std::string::iterator xar = str->end();
	xar--;

	// Missing the logic to append the directive's values to the Class
	// location is a block, need to implement verify all the block
	if (*xar != ';')
		throw std::runtime_error("Directive line must end in ;");
}
