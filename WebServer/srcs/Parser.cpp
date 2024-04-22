/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 10:00:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/17 15:21:22 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Server.hpp"
#include "utils.hpp"

//Prototypes:
static bool isTokenInDirectives(const std::string& token, const std::string& block);

//static members need to be defined outside the class.
std::vector<Server> Parser::_servers;
std::map<std::string, std::vector<std::string> > Parser::_directives;
std::map<std::string, std::map<std::string, std::vector<std::string> > > Parser::_locations;
const char* Parser::server_directives[] = {"listen", "server_name", "error_page", NULL};
const char* Parser::location_directives[] = {"root", "index", "auto_index", "client_max_body_size",
											 "allow_methods", "cgi_pass", "redirect", NULL};

Parser::Parser(void) {}

Parser::~Parser(void) {}

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
	bool is_server_closed;
	bool is_location_closed = true;

	if (inputFile.is_open()) {
		// Peek looks at the next character in the stream. If peek returns EOF the file is empty.
		if (inputFile.peek() == std::ifstream::traits_type::eof())
			throw std::runtime_error("The config file doesn't have content");

		//Outside the server block
		while (std::getline(inputFile, line)) {
			std::stringstream ss(line);

			// Only empty lines and >isolated< comments are allowed outside the block
			// Isolated commentaries means a content commented line.
			if (!(ss >> token) || token[0] == '#') continue;
			if (token != "server")
				throw std::runtime_error("Invalid Server block");
			//After "server" found.
			if (!(ss >> token) || token != "{") {
				if (token == "{}") {
					is_server_closed = true;
					break;
				}
				else throw std::runtime_error("Server block must be opened with `{");
			}
			//Inside the SERVER BLOCK
			is_server_closed = false;
			while (std::getline(inputFile, line) && is_location_closed) {
				std::stringstream ss(line);
				if (!(ss >> token) || token[0] == '#') continue;
				//Closing server block
				if (token == "}") {
					is_server_closed = true;
					break;
				}
				if (!isTokenInDirectives(token, "server"))
					throw std::runtime_error(token + " is an invalid server directive");
				std::vector<std::string> vec(Utils::extractValues(line));

				if (token == "location") {
					Parser::parsingLocationBlock(vec);
					is_location_closed = false;
					//URI - Uniform Resource Identifier
					std::string uri(vec[0]);
					// After parsingLocationBlock the line must be -> location /* { or location /* {}
					(ss >> token); (ss >> token);
					// It is an empty location
					if (token == "{}") {
						is_location_closed = true;
						Parser::_locations[uri]["index"] = Utils::splitStringToVector("index.html;");
						continue;
					}
					//Inside the location block
					while (std::getline(inputFile, line)) {
						std::stringstream ss(line);
						if (!(ss >> token) || token[0] == '#') continue;
						if (token == "}") {
							if (Parser::_locations.empty()) {
								Parser::_locations[uri]["index"] = Utils::splitStringToVector("index.html;");
							}
							is_location_closed = true;
							break; //Closing location block
						}
						// New block opened
						vec.clear();
						if (!isTokenInDirectives(token, "location")) // missing location block
							throw std::runtime_error(token + " is an invalid location directive");
						vec = Utils::extractValues(line);
						if (vec.size() == 1 && vec[0] == ";")
							throw std::runtime_error("Directive " + token + " is empty");
						Parser::parsingDirectives(token, vec, Parser::_locations[uri]);
						Parser::_locations[uri][token] = vec;
					}
				} else { // Server directives not location block
					Parser::parsingDirectives(token, vec, Parser::_directives);
					Parser::_directives[token] = vec;
				}
			}
			if (!is_location_closed)
				throw std::runtime_error("Error: Location block isn't closed");
			// The object is created, then push back creates a copy, then the temporary server object is destructed
			_servers.push_back(Server(_directives, _locations));
			_directives.clear(); _locations.clear();
		}
	} else
		throw std::runtime_error("Cannot open the config file");
	if (!is_server_closed)
		throw std::runtime_error("Error: Server block isn't closed!");
	inputFile.close();
	std::cout << ANSI_COLOR_CYAN << "\n***************************************************************************\n";
	std::cout << "******************************* " << ANSI_COLOR_GREEN << "PARSER - OK" << ANSI_COLOR_CYAN << " *******************************\n";
	std::cout << "***************************************************************************\n\n" << ANSI_COLOR_RESET;
	Utils::printServer(_servers);
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
	// The last element os the location line has to be "{" or "{}"
	if ((*end) != "{" && (*end) != "{}")
		throw std::runtime_error("Location block must has a opening { or {}");
	--end;
	// Location has only 1 URI, the URI must starts and ends with '/'
	if ((*end)[0] != '/' && (*end)[(*end).size() - 1] != '/')
		throw std::runtime_error("URI must begin and start with /");
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

