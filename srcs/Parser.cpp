/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 10:00:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/01/30 17:44:07 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Parser.hpp"
#include "../includes/utils.hpp"

Parser::Parser(void) {}

Parser::~Parser(void) {}

void
Parser::parsingConfigFile(const std::string &config_file) {
	// Passing an empty string in ifstream parameter will result in undefined behaviour.
	if (config_file.empty())
		throw std::runtime_error("The config file cannot be empty");
	std::ifstream inputFile(config_file.c_str());

	if (inputFile.is_open()) {
		// Peek looks at the next character in the stream. If peek returns EOF the file is empty.
		if (inputFile.peek() == std::ifstream::traits_type::eof())
			throw std::runtime_error("The config file hasn't content");
		std::string line;
		std::string token;
		std::stringstream ss;

		while (std::getline(inputFile, line)) {
			ss << line; ss >> token;
			if (token.empty() || token[0] == '#')
				continue; // It means that line is empty
			if (token == "server") {
				ss >> token;
				if (!token.empty() && token[0] == '{') {
					while (std::getline(inputFile, line)) {
						ss << line; ss >> token;
						if (token == "}") //Server block closing
							break;
						if (token.empty() || token[0] == '#')
							continue;
						if (!isTokenInDirectives(token)) // missing location block
							throw std::runtime_error("Invalid server directive");
						Parser::parsingDirectives(splitString(line));
					}
				} else
					throw std::runtime_error("Server block must be opened with `{");
			} else
				throw std::runtime_error("Invalid block");
		}
	} else
		throw std::runtime_error("Cannot open the config file");
}

void
Parser::parsingDirectives(const std::vector<std::string>& split) {
	size_t size = split.size();

	if (size == 1)
		throw std::runtime_error(split[0] + " doesn't have values");
	for (size_t i = 1; i < size; i++) {
		// Missing the logic to append the directive's values to the Class
		// location is a block, need to implement verify all the block
		if (i == (size - 1))
			(split[i].back() == ';') ? return : throw std::runtime_error("The directive's line must end in `;'")
	}
}