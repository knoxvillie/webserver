/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 10:00:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/02/07 17:31:22 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(void) {}

Parser::~Parser(void) {}

//static members need to be defined outside the class.
std::map<std::string, std::vector<std::string> > Parser::_directives;

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
			std::stringstream ss(line);
			if (!(ss >> token) || token[0] == '#') continue;
			if (token != "server")
				throw std::runtime_error("Invalid block");
			if (!(ss >> token) || token[0] != '{')
				throw std::runtime_error("Server block must be opened with `{");
			while (std::getline(inputFile, line)) {
				std::stringstream ss(line);
				if (!(ss >> token) || token[0] == '#') continue;
				if (token == "}") // Server block closing
					break;
				if (!isTokenInDirectives(token)) // missing location block
					throw std::runtime_error(token + " is an invalid server directive");
				std::vector<std::string> vec(splitString(line));
				Parser::parsingDirectives(token, vec);
				Parser::_directives[token] = vec;
			}
		}
	} else
		throw std::runtime_error("Cannot open the config file");
	inputFile.close();
	printMap(_directives);
}

void
Parser::parsingDirectives(const std::string& directive, std::vector<std::string>& split) {
	if (split.empty())
		throw std::runtime_error(directive + " doesn't have values");
	std::vector<std::string>::iterator str = split.end(); --str;
	std::string::iterator xar = str->end(); --xar;
	// Missing the logic to append the directive's values to the Class
	// location is a block, need to implement verify all the block
	if (*xar != ';')
		throw std::runtime_error("Directive line must end in ;");
}
