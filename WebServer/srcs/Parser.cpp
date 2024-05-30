/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/24 13:59:48 by kfaustin          #+#    #+#             */
/*   Updated: 2024/05/30 13:44:42 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"
#include "Server.hpp"
#include "utils.hpp"

//Prototypes:
static bool isTokenInDirectives(const std::string& token, const std::string& block);
static void parsingDirectives(const std::string& directive, std::vector<std::string>& vec, std::map<std::string, std::vector<std::string> >& map);
static void parsingLocationBlock(std::vector<std::string>& vec);
static void printParserPattern(void);

//static members need to be defined outside the class.
std::vector<Server> Parser::_servers;
std::map<std::string, std::vector<std::string> > Parser::_directives;
std::map<std::string, std::map<std::string, std::vector<std::string> > > Parser::_locations;
const char* Parser::server_directives[] = {"host", "port", "server_name", "error_page", NULL};
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
		throw std::runtime_error("ERROR - Parser: The configuration file is empty");
	std::ifstream inputFile(config_file.c_str());
	std::string token;
	std::string line;
	bool is_server_closed = false;
	bool is_location_closed = true;

	if (inputFile.is_open()) {
		// Peek looks at the next character in the stream. If peek returns EOF the file is empty.
		if (inputFile.peek() == std::ifstream::traits_type::eof())
			throw std::runtime_error("ERROR - Parser: The configuration file doesn't have content");

		// Inside the file, outside the server block
		while (std::getline(inputFile, line)) {
			std::stringstream ss(line);

			// Skip black lines and comments.
			if (!(ss >> token) || token[0] == '#') continue;
			// The first special word must be a "server"
			if (token != "server")
				throw std::runtime_error("ERROR - Parser: Expected 'server' but but found " + token);
			// After "server" was find, token must be a start '{' or start and close block '{}'
			if (!(ss >> token) || token != "{") {
				if (token == "{}") {
					is_server_closed = true;
					break;
				}
				else throw std::runtime_error("ERROR - Parser: Server block must be opened with '{'");
			}
			// Inside the SERVER BLOCK
			is_server_closed = false;
			while (std::getline(inputFile, line) && is_location_closed) {
				std::stringstream ss(line);
				// Skip blank lines and comments.
				if (!(ss >> token) || token[0] == '#') continue;
				//Closing server block
				if (token == "}") {
					is_server_closed = true;
					break;
				}
				// Validating a SERVER directive
				if (!isTokenInDirectives(token, "server"))
					throw std::runtime_error("ERROR - Parser: Invalid server directive " + token);
				std::vector<std::string> vec(Utils::extractValues(line));

				// If the directive is a LOCATION BLOCK
				if (token == "location") {
					::parsingLocationBlock(vec);
					is_location_closed = false;
					// URI - Uniform Resource Identifier
					std::string uri(vec[0]);
					// After parsingLocationBlock the line must be -> location /* { or location /* {} (instead of /* can also be .*)
					(ss >> token); (ss >> token);
					// Location is empty
					if (token == "{}") {
						is_location_closed = true;
						// Creating a standard "index" directive to prevent location to be empty.
						Parser::_locations[uri]["index"] = Utils::splitStringToVector("index.html;");
						// Continue to the next server block line
						continue;
					}
					// Inside the LOCATION BLOCK
					while (std::getline(inputFile, line)) {
						std::stringstream ss(line);
						// Skipping blank lines or comments.
						if (!(ss >> token) || token[0] == '#') continue;
						if (token == "}") {
							// Checks if location block is empty, then adding a standard "index" value to prevent errors.
							if (Parser::_locations.empty()) {
								Parser::_locations[uri]["index"] = Utils::splitStringToVector("index.html;");
							}
							is_location_closed = true;
							// Closing location block
							break;
						}
						vec.clear();
						if (!isTokenInDirectives(token, "location"))
							throw std::runtime_error("ERROR - Parser: Invalid location directive " + token);
						vec = Utils::extractValues(line);
						::parsingDirectives(token, vec, Parser::_locations[uri]);
						Parser::_locations[uri][token] = vec;
					}
				}
				// Server directives, not a location block
				else {
					::parsingDirectives(token, vec, Parser::_directives);
					Parser::_directives[token] = vec;
				}
			}
			if (!is_location_closed)
				throw std::runtime_error("ERROR - Parser: Location block isn't closed");
			// The object is created, then push back creates a copy, then the temporary server object is destructed
			_servers.push_back(Server(_directives, _locations));
			_directives.clear(); _locations.clear();
		}
	} else
		throw std::runtime_error("ERROR - Parser: Cannot open the configuration file");
	if (!is_server_closed)
		throw std::runtime_error("ERROR - Parser: Server block isn't closed!");
	inputFile.close();
	printParserPattern();
	Utils::printServer(_servers);
}

// Getters

std::vector<Server>&
Parser::getServers(void) {return (_servers);}

// Static Functions

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

static void
parsingDirectives(const std::string& directive, std::vector<std::string>& vec, std::map<std::string, std::vector<std::string> >& map) {
	if (vec.empty())
		throw std::runtime_error("ERROR - Parser: Empty directive " + directive);
	if (vec.size() == 1 && vec[0] == ";")
		throw std::runtime_error("ERROR - Parser: Directive " + directive + " value is empty");

	std::vector<std::string>::iterator str = vec.end(); --str;
	std::map<std::string, std::vector<std::string> >::const_iterator it = map.find(directive);

	// Checking the last char of the last string in vec
	if ((*str)[(*str).size() - 1] != ';')
		throw std::runtime_error("ERROR - Parser: All Directive lines must be ended in ';'");
	// Checking repeated directives in map (server and location) included.
	if (it != map.end()) {
		// Checking if is a server or a location directive to print assertively
		for (int i = 0; Parser::server_directives[i]; i++) {
			if (std::strcmp(directive.c_str(), Parser::server_directives[i]) == 0)
				throw std::runtime_error("ERROR - Parser: Server block has multiples directives: " + directive);
		}
		throw std::runtime_error("ERROR - Parser: Location block has multiples directives: " + directive);
	}
}

static void
parsingLocationBlock(std::vector<std::string>& vec) {
	if (vec.size() != 2)
		throw std::runtime_error("ERROR - Parser: Invalid declaration of a location block");
	// even if some vec[string] is empty, is ok to index it. No segfault
	std::vector<std::string>::const_iterator end = vec.end(); --end;
	// The last element os the location line has to be "{" or "{}"
	if ((*end) != "{" && (*end) != "{}")
		throw std::runtime_error("ERROR - Parser: Invalid Location syntax, after URI is expected '{' or '{}' in: " + vec[1]);
	--end;
	// Location has only 1 URI, the URI must starts and ends with '/'
	if ((*end)[0] != '/' || (*end)[(*end).size() - 1] != '/') {
		// Location can also be an extension, starting with "." and containing up to 4 characters (5 with the dot)
		if (end->size() <= 1 || end->size() > 5 || (*end)[0] != '.' )
			throw std::runtime_error("ERROR - Parser: Invalid URI syntax. URI must start and end with '/' in " + vec[1]);
	}
}

static void
printParserPattern(void) {
	std::cout << ANSI_COLOR_CYAN << "\n-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
	std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- " << ANSI_COLOR_GREEN << "PARSER - OK" << ANSI_COLOR_CYAN << " -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";
	std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n\n" << ANSI_COLOR_RESET;
}