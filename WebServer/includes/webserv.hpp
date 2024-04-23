/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:44:36 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/23 15:39:45 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

// ========================
//  My Namespaces
// ========================
//#include "Utils.hpp"

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
#include <sys/stat.h>
#include <ctime>

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

class Global {
	public:
		static std::string pwd;
};

struct t_location {
	std::string location_name;
	std::string root;
	std::string index;
	bool auto_index;
	long CMaxBodySize; //bytes
	std::vector<std::string> allow_methods;
	std::string cgi_pass;
	bool redirect_is_extern;
	std::string redirect;
};

//	Prototypes
class Server;

// HTTP Request
struct t_request {
	Server *server;					// The server to which the request was sent
	
	// Could be useful to store the location of the request too ?

	std::string full; 				// The full request
	std::string first_line; 		// <method> <status code> <http version>
	std::string header; 			// Header of the request
	std::string body; 				// body of the request
	std::string unparsed_url;		// URL before checking for QUERY_STRING and PATH_INFO
	std::string url;				// URL without QUERY_STRING and PATH_INFO
	std::string content;
	std::string request_line;
	std::string header;
	std::string body;
	std::string url;
	std::string http_version;
	
	std::string method;				// Http method
	std::string file_path;			// Path to the file (for cases when its the index)
	std::map<std::string, std::string> headerMap; // Map of header directives to attributes
	
	std::string path_info;			// PATH_INFO env varible for CGI
	std::string query_string;		// QUERY_STRING env variable for CGI
	bool isCGI;						// Whether or not the file requested is a CGI
};

// ======================
//       Functions       
// ======================

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
bool isDirectory(const std::string& path);


// ======================
//       Templates       
// ======================
template <typename T>
void printMapVec(const std::map<T, std::vector<T> >& myMap) {
	//Using const_iterator instead of iterator because I don't intend to modify the elements of the container.
	for (typename std::map<T, std::vector<T> >::const_iterator it = myMap.begin(); it != myMap.end(); it++) {
		std::cout << ANSI_COLOR_GREEN << "|Directive: " << ANSI_COLOR_YELLOW << std::left << std::setw(25) << it->first << ANSI_COLOR_RED << ":: ";
		for (typename std::vector<T>::const_iterator ut = it->second.begin(); ut != it->second.end(); ut++)
			std::cout << ANSI_COLOR_YELLOW << *ut << " ";
		std::cout << ANSI_COLOR_RESET << std::endl;
	}
}

template <typename T>
void printMapMapVec(const std::map<T, std::map<T, std::vector<T> > >& myMap) {
	for (typename std::map<T, std::map<T, std::vector<T> > >::const_iterator it = myMap.begin(); it != myMap.end(); it++) {
		std::cout << ANSI_COLOR_GREEN "\n- Location: " << ANSI_COLOR_YELLOW << it->first << std::endl;
		printMapVec(it->second);
		std::cout << std::endl;
	}
}

#endif
