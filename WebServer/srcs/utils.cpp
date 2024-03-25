/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 12:51:54 by kfaustin          #+#    #+#             */
/*   Updated: 2024/03/25 11:53:00 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "Server.hpp"
#include "cluster.hpp"

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

bool
isStringNum(const std::string& string) {
	if (string.empty())
		return false;
	for (size_t i = 0; i < string.size(); i++) {
		if (string[i] < '0' || string[i] > '9')
			return false;
	}
	return true;
}

uint32_t ipParserHtonl(const std::string& ip_address) {
	char* endptr;
	uint32_t bytes = 0;
	std::string token;
	std::istringstream iss(ip_address);

	for (short i = 0; i < 4; i++) {
		if (!std::getline(iss, token, '.'))
			throw  std::runtime_error("Error: Invalid IP address format");
		long octet = std::strtol(token.c_str(), &endptr, 10);

		if (*endptr != '\0')
			throw std::runtime_error("Error: IP address conversion failed octet: " + token);
		if (octet < 0 || octet > 255)
			throw std::runtime_error("Error: Invalid IP address, octet out of range");
		bytes |= static_cast<uint32_t>(octet) << ((3 - i) * 8);
	}
	return (bytes);
}

void signal_handler(int signum) {
	GPS;
	
	if (signum == SIGINT) {
		MLOG("here");
		deleteServers();
	}
}
