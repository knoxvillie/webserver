/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 12:51:54 by kfaustin          #+#    #+#             */
/*   Updated: 2024/02/08 17:10:57 by kfaustin         ###   ########.fr       */
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
isTokenInDirectives(const std::string& token, const std::string& block) {
	const char* directives[] = {"listen", "server_name", "host", "root", "index",
								"charset", "access_log","error_log", "error_page",
								"location", "client_max_body_size" , NULL};
	if (block == "location") {
		const char* directives[] = {"autoindex", "allow_methods", "cgi_pass"};
		(void)directives;
	}
	for (int i = 0; directives[i]; i++) {
		if (token == directives[i])
			return (true);
	}
	return (false);

//	std::vector<std::string> server_directives(vectorInitializer(directives));
//	for (size_t i = 0; i < server_directives.size(); i++) {
//		if (token == server_directives[i])
//			return (true);
//	}
}

std::vector<std::string>
splitString (const std::string& input) {
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string token;

	ss >> token;
	for (;ss >> token;)
			result.push_back(token);
	return (result);
}


