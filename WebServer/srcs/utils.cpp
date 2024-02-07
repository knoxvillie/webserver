/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 12:51:54 by kfaustin          #+#    #+#             */
/*   Updated: 2024/02/07 15:10:55 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

std::vector<std::string>
vectorInitializer(const char** list) {
	std::vector<std::string> result;
	for (int i = 0; list[i]; i++)
		result.push_back(std::string(list[i]));
	return (result);
}

bool
isTokenInDirectives(const std::string& token) {
	// Maybe this should be stored in the Parser class ?
	const char* directives[] = {"listen", "server_name", "root", "index", "charset",
								"access_log","error_log", "error_page", "location", NULL};
	std::vector<std::string> server_directives(vectorInitializer(directives));

	for (size_t i = 0; i < server_directives.size(); i++) {
		if (token == server_directives[i])
			return (true);
	}
	return (false);
}

std::vector<std::string>
splitString (const std::string& input, char delimiter) {
	std::vector<std::string> result;
	std::istringstream stream(input);
	std::string token;

	while(std::getline(stream, token, delimiter))
		result.push_back(token);
	return (result);
}
