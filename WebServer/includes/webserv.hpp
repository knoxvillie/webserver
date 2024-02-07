/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:44:36 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/07 14:42:38 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#ifdef DEBUG
# define MLOG(a,b) std::cout << "DEBUG" << std::endl;
#else
# define MLOG(a,b) do {} while(false);
#endif

#ifndef MERROR 
# define MERROR(message) std::cout << "Error: " << message << std::endl; exit(1);
#endif


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
#include <map>
#include <stdexcept>
#include <arpa/inet.h>


// ======================
//       Functions       
// ======================

//	Prototypes
bool isTokenInDirectives(const std::string& token);
std::vector<std::string> splitString (const std::string& input);
std::vector<std::string> vectorInitializer(const char **list);

#endif