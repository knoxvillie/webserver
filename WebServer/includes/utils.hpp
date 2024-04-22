/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 12:34:57 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/16 16:10:30 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef UTILS_HPP
# define UTILS_HPP

#include "webserv.hpp"
#include "Server.hpp"

extern volatile sig_atomic_t gEndLoop;

namespace Utils {
	std::vector<std::string> vectorInitializer(const char**);
	// Split for general propose, delimiter one or more spaces.
	std::vector<std::string> splitStringToVector(const std::string&);
	// Split config file directive line, values only
	std::vector<std::string> extractValues(const std::string&);
	void printServer(std::vector<Server>&);
	bool isStringUnsignedInt(const std::string&);
	uint32_t ipParserHtonl(const std::string&);
	std::string intToString(int);
	std::string getValueFromEnv(char**, const std::string&);
	void signal_handler(int);
	bool isDirectory(const std::string& str);
	int isRegularFile(const std::string& path);
	void createStyleIfNotExists(void);
	void createGenericErrorPage(std::ostringstream&, int&);
	void createListingPage(std::stringstream&, const std::string&);
}

#endif