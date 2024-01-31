/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 12:51:54 by kfaustin          #+#    #+#             */
/*   Updated: 2024/01/31 14:19:43 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/utils.hpp"

bool
isTokenInDirectives(const std::string& token) {
	std::vector<std::string> server_directives = SERVER_DIRECTIVES;
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