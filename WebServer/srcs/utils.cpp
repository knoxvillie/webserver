/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 12:51:54 by kfaustin          #+#    #+#             */
/*   Updated: 2024/02/12 17:15:12 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Server.hpp"

std::vector<std::string>
vectorInitializer(const char** list) {
	std::vector<std::string> result;
	for (int i = 0; list[i]; i++)
		result.push_back(std::string(list[i]));
	return (result);
}

// Split for general propose, delimiter one or more spaces.
std::vector<std::string>
splitStringToVector(const std::string& string) {
	std::vector<std::string> split;
	std::stringstream ss(string);
	std::string token;
	for (;ss >> token;)
		split.push_back(token);
	return (split);
}


// Split config file directive line, values only
std::vector<std::string>
extractValues(const std::string& input) {
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string token;

	ss >> token; //skipping the (directive) the first element of input.
	for (;ss >> token;)
			result.push_back(token);
	return (result);
}

void
printServer(std::vector<Server>& serverList) {
	for (size_t i = 0; i < serverList.size(); i++) {
		std::cout << "\nServer " << i << " :" << std::endl;
		printMapVec(serverList[i].getServerDirectives());
		if (serverList[i].getLocationDirectives().empty())
			std::cout << "None Location block" << std::endl;
		else
			printMapMapVec(serverList[i].getLocationDirectives());
	}
}



