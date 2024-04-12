/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:44:36 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/12 15:10:29 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// ========================
//  C++ Standard Libraries
// ========================

#include <iostream>
#include <iomanip>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib> //stdlib deprecated
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <map>
#include <stdexcept>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <csignal>
#include <fcntl.h>
#include <dirent.h>

// ========================
// 		Macros and struct
// ========================

class FuncLogger;

// ANSI escape codes for text color
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_CYAN    "\033[1;36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_YELLOW  "\033[1;33m"

#ifdef DEBUG
# define MLOG(str) std::cout << str << std::endl;
# define GPS FuncLogger gps_obj(__FILE__, __FUNCTION__, __LINE__);
#else
# define MLOG(str) do {} while(false);
# define GPS do {} while(false);
#endif

#ifndef MERROR 
# define MERROR(message) std::cerr << "Error: " << message << std::endl; exit(1);
#endif

class FuncLogger {
	private:
		const char* file;
		const char* func;
		const int line;

	public:
		FuncLogger(const char* file, const char* func, const int line) : file(file), func(func), line(line) {
			std::cout << "[" << ANSI_COLOR_GREEN << "IN" << ANSI_COLOR_RESET << "]" << "  " << ANSI_COLOR_CYAN << "INFO: " << ANSI_COLOR_RESET << func << ANSI_COLOR_YELLOW << ":" << ANSI_COLOR_RESET << line << " - " << ANSI_COLOR_GREEN << file << ANSI_COLOR_RESET << std::endl;
		};
		~FuncLogger(void) {
			(void)line;
			std::cout << "[" << ANSI_COLOR_RED << "OUT" << ANSI_COLOR_RESET << "]" << " " << ANSI_COLOR_CYAN << "INFO: " << ANSI_COLOR_RESET << func << " - " << ANSI_COLOR_GREEN << file << ANSI_COLOR_RESET << std::endl;
		};
};

struct t_location {
	std::string location_name;
	std::string root;
	std::string index;
	bool auto_index;
	short CMaxBodySize;
	std::vector<std::string> allow_methods;
	std::string cgi_pass;
	std::string redirect;
};

// HTTP Request
struct t_request {
	std::string first_line;
	std::string full;
	std::string header;
	std::string body;
	std::string url;
	
	std::string method;
	std::string file_path;
	std::map<std::string, std::string> headerMap;
};

// ======================
//       Functions       
// ======================

//	Prototypes
class Server;

//	Utils.cpp
std::vector<std::string> splitStringToVector(const std::string&);
std::vector<std::string> extractValues (const std::string&);
std::vector<std::string> vectorInitializer(const char**);
void printServer(std::vector<Server>&);
bool isStringUnsignedInt(const std::string&);
uint32_t ipParserHtonl(const std::string&);
std::string intToString(int number);
std::string getValueFromEnv(char** env, const std::string&);
void signal_handler(int signum);


// ======================
//       Templates       
// ======================
template <typename T>
void printMapVec(const std::map<T, std::vector<T> >& myMap) {
	//Using const_iterator instead of iterator because I don't intend to modify the elements of the container.
	for (typename std::map<T, std::vector<T> >::const_iterator it = myMap.begin(); it != myMap.end(); it++) {
		std::cout << "Directive: " << std::left << std::setw(25) << it->first << "-> ";
		for (typename std::vector<T>::const_iterator ut = it->second.begin(); ut != it->second.end(); ut++)
			std::cout << *ut << " ";
		std::cout << std::endl;
	}
}

template <typename T>
void printMapMapVec(const std::map<T, std::map<T, std::vector<T> > >& myMap) {
	for (typename std::map<T, std::map<T, std::vector<T> > >::const_iterator it = myMap.begin(); it != myMap.end(); it++) {
		std::cout << "\nLocation: " << it->first << std::endl;
		printMapVec(it->second);
		std::cout << std::endl;
	}
}

#endif
