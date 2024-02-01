/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 09:36:33 by kfaustin          #+#    #+#             */
/*   Updated: 2024/01/30 17:52:08 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#pragma once

#ifdef DEBUG
# define MESSAGE(msg, svar) {std::cout << "#DEBUG: " << msg << " -> '" << svar << "'" << std::endl;}
#else
# define MESSAGE
#endif

# define SERVER_DIRECTIVES { \
	"listen", "server_name", "root", "index", "charset", \
	"access_log", "error_log", "error_page", "location"};

//C++ Standard Libraries
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

#endif