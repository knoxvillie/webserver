/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 10:00:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/02/12 13:28:15 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser(void) {}

Parser::~Parser(void) {}

//static members need to be defined outside the class.
std::map<std::string, std::vector<std::string> > Parser::_directives;
std::map<std::string, std::map<std::string, std::vector<std::string> > > Parser::_locations;

void
Parser::parsingConfigFile(const std::string &config_file) {
	// Passing an empty string in ifstream parameter will result in undefined behaviour.
	if (config_file.empty())
		throw std::runtime_error("The config file cannot be empty");
	std::ifstream inputFile(config_file.c_str());
	std::string token;
	std::string line;

	if (inputFile.is_open()) {
		// Peek looks at the next character in the stream. If peek returns EOF the file is empty.
		if (inputFile.peek() == std::ifstream::traits_type::eof())
			throw std::runtime_error("The config file doesn't have content");

		//Outside the server block
		while (std::getline(inputFile, line)) {
			std::stringstream ss(line);

			//Only empty lines and >isolated< commentaries are allowed outside the block
			if (!(ss >> token) || token[0] == '#') continue;
			if (token != "server")
				throw std::runtime_error("Invalid block");
			if (!(ss >> token) || token[0] != '{')
				throw std::runtime_error("Server block must be opened with `{");
			//Inside the server block
			while (std::getline(inputFile, line)) {
				std::stringstream ss(line);
				if (!(ss >> token) || token[0] == '#') continue;
				if (token == "}") break; //Closing server block
				if (!isTokenInDirectives(token, "server"))
					throw std::runtime_error(token + " is an invalid server directive");
				std::vector<std::string> vec(extractValues(line));

				if (token == "location") {
					Parser::parsingLocationBlock(vec);
					//Inside the location block
					while (std::getline(inputFile, line)) {
						std::stringstream ss(line);
						if (!(ss >> token) || token[0] == '#') continue;
						if (token == "}") break; //Closing location block
						//URI - Uniform Resource Identifier
						std::string uri(vec[0]);

						vec.clear();
						vec = extractValues(line);
						if (!isTokenInDirectives(token, "location")) // missing location block
							throw std::runtime_error(token + " is an invalid location directive");
						Parser::parsingDirectives(token, vec);
						Parser::_locations[uri][token] = vec; //shit is crazy my man
					}
				} else {
					Parser::parsingDirectives(token, vec);
					Parser::_directives[token] = vec;
				}
			}
		}
	} else
		throw std::runtime_error("Cannot open the config file");
	if (token != "}")
		throw std::runtime_error("all blocks must be closed");
	inputFile.close();
	printMapMapVec(_locations);
	printMapVec(_directives);
}

void
Parser::parsingDirectives(const std::string& directive, std::vector<std::string>& vec) {
	if (vec.empty())
		throw std::runtime_error(directive + " doesn't have values");
	std::vector<std::string>::iterator str = vec.end(); --str;
	std::string::iterator xar = str->end(); --xar;
	// Missing the logic to append the directive's values to the Class
	// location is a block, need to implement verify all the block
	if (*xar != ';')
		throw std::runtime_error("Directive line must end in ;");
}

/*
	location [ = | ~ | ~* | ^~ ] uri {
	...
	}
     location: The keyword to start a location block.
    [ = | ~ | ~* | ^~ ]: Optional modifiers that affect how Nginx interprets the URI. Common modifiers include:
        =: Exact match.
        ~: Case-sensitive regular expression match.
        ~*: Case-insensitive regular expression match.
        ^~: Non-regular expression match, takes precedence over regular expression matches.
    uri: The string or regular expression representing the location to match.
 * */
void
Parser::parsingLocationBlock(std::vector<std::string>& vec) {
	// I don't know if the location block is allowed to have more than one URI.
	if (vec.size() != 2)
		throw std::runtime_error("Invalid location block, URI or {");
	// even if some vec[string] is empty, is ok to index it. No segfault
	std::vector<std::string>::const_iterator end = vec.end(); --end;
	// The last element os the location line has to be '{'
	if (end->size() != 1 || (*end)[0] != '{')
		throw std::runtime_error("Location block must has a opening {");
	// for used if location block can receive more than one URI
	for (std::vector<std::string>::const_iterator it = vec.begin(); it != end; ++it) {
		//URI has to start with '/' or '.' if CGI
		if ((*it)[0] != '/' && (*it)[0] != '.')
			throw std::runtime_error("URI must begin with /");
	}
}