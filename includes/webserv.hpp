/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:44:36 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/01 12:47:21 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifdef DEBUG
# define MESSAGE(a,b) std::cout << "DEBUG" << std::endl;
#else
# define MESSAGE
#endif

# define SERVER_DIRECTIVES { \
	"listen", "server_name", "root", "index", "charset", \
	"access_log", "error_log", "error_page", "location"};


// ========================
//  C++ Standard Libraries	
// ========================

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

#include <arpa/inet.h>


// ======================
//		 Functions		
// ======================

//	Prototypes
bool	isTokenInDirectives(const std::string& token);
std::vector<std::string> splitString (const std::string& input, char delimiter);

//	error.cpp
void exitWithError(const std::string &errorMessage);
void log(const std::string &message);
