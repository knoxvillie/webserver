/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 10:00:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/02 15:46:03 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Server.hpp"

//Prototypes:
static bool isTokenInDirectives(const std::string& token, const std::string& block);

//static members need to be defined outside the class.
std::string Parser::_pwd;
std::vector<Server> Parser::_servers;
std::map<std::string, std::vector<std::string> > Parser::_directives;
std::map<std::string, std::map<std::string, std::vector<std::string> > > Parser::_locations;
const char* Parser::server_directives[] = {"listen", "server_name", "error_page", NULL};
const char* Parser::location_directives[] = {"index", "root", "auto_index", "client_max_body_size", "allow_methods", "cgi_pass", NULL};

Parser::Parser(void) {}

Parser::~Parser(void) {}

void
Parser::setPWD(char **env) {
	Parser::_pwd = getValueFromEnv(env, "PWD");
	if (Parser::_pwd.empty())
		throw std::runtime_error("Error: PWD is empty");
}

// Methods to parser the config file
void
Parser::parsingConfigFile(const std::string &config_file) {
	GPS;
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

			// Only empty lines and >isolated< comments are allowed outside the block
			// Isolated commentaries means a full commented line.
			if (!(ss >> token) || token[0] == '#') continue;
			if (token != "server")
				throw std::runtime_error("Invalid block");
			if (!(ss >> token) || token != "{")
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
					//URI - Uniform Resource Identifier
					std::string uri(vec[0]);

					//Inside the location block
					while (std::getline(inputFile, line)) {
						std::stringstream ss(line);
						if (!(ss >> token) || token[0] == '#') continue;
						if (token == "}") {
							if (Parser::_locations.empty()) {
								Parser::_locations[uri]["index"] = splitStringToVector("index.html;");
							}
							break; //Closing location block
						}

						vec.clear();
						if (!isTokenInDirectives(token, "location")) // missing location block
							throw std::runtime_error(token + " is an invalid location directive");
						vec = extractValues(line);
						Parser::parsingDirectives(token, vec, Parser::_locations[uri]);
						Parser::_locations[uri][token] = vec;
					}
				} else { // Server directives not location block
					Parser::parsingDirectives(token, vec, Parser::_directives);
					Parser::_directives[token] = vec;
				}
			}
			// The object is created, then push back creates a copy, then the temporary server object is destructed
			_servers.push_back(Server(_directives, _locations));
			_directives.clear(); _locations.clear();
		}
	} else
		throw std::runtime_error("Cannot open the config file");
	if (line != "}")
		throw std::runtime_error("all blocks must be closed");
	inputFile.close();
	printServer(_servers);
}

void
Parser::parsingDirectives(const std::string& directive, std::vector<std::string>& vec, std::map<std::string, std::vector<std::string> >& map) {
	if (vec.empty())
		throw std::runtime_error(directive + " doesn't have values");
	//std::vector<std::string>::iterator it = vec.begin();
	std::vector<std::string>::iterator str = vec.end(); --str;
	std::string::iterator xar = str->end(); --xar;

	if (*xar != ';')
		throw std::runtime_error("Directive line must end in ;");
	std::map<std::string, std::vector<std::string> >::const_iterator it = map.find(directive);
	if (it != map.end())
		throw std::runtime_error("Parser Error: Server block has multiples directives: " + directive);
}

void
Parser::parsingLocationBlock(std::vector<std::string>& vec) {
	// I don't know if the location block is allowed to have more than one URI.
	if (vec.size() != 2)
		throw std::runtime_error("Invalid location block, URI or {");
	// even if some vec[string] is empty, is ok to index it. No segfault
	std::vector<std::string>::const_iterator end = vec.end(); --end;
	// The last element os the location line has to be '{'
	if ((*end) != "{") // check if \n is included in >> extract
		throw std::runtime_error("Location block must has a opening {");
	// for used if location block can receive more than one URI
	for (std::vector<std::string>::const_iterator it = vec.begin(); it != end; ++it) {
		//URI has to start with '/' or '.' if CGI
		if ((*it)[0] != '/' && (*it)[0] != '.')
			throw std::runtime_error("URI must begin with /");
	}
}

// Getters

std::vector<Server>&
Parser::getServers(void) {return (_servers);}

// Related functions

static bool
isTokenInDirectives(const std::string& token, const std::string& block) {
	if (token == "location" && block == "server")
		return (true);
	const char** directives = (block == "location" ? Parser::location_directives : Parser::server_directives);

	for (int i = 0; directives[i]; i++) {
		if (token == directives[i])
			return (true);
	}
	return (false);
}
