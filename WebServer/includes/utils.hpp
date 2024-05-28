/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 12:34:57 by kfaustin          #+#    #+#             */
/*   Updated: 2024/05/28 12:34:42 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef UTILS_HPP
# define UTILS_HPP

#include "webserv.hpp"
#include "Server.hpp"
#include "Response.hpp"

extern volatile sig_atomic_t gEndLoop;

namespace Utils {
	std::vector<std::string> vectorInitializer(const char**);
	// Split for general propose, delimiter one or more spaces.
	std::vector<std::string> splitStringToVector(const std::string&);
	// Split config file directive line, values only
	std::vector<std::string> extractValues(const std::string&);
	std::string serverPortToString (std::vector<uint16_t> vec);
	void printServer(std::vector<Server>&);
	bool isStringUnsignedInt(const std::string&);
	uint32_t ipParserHtonl(const std::string&);
	std::string intToString(int);
	std::string getValueFromEnv(char**, const std::string&);
	void signal_handler(int);
	bool isDirectory(const std::string& str);
	int isRegularFile(const std::string& path);
	void createStyleIfNotExists(void);
	const std::string createGenericErrorPage(int status_code, const std::string& message);
	void createListingPage(std::stringstream&, const std::string&);
}

#endif