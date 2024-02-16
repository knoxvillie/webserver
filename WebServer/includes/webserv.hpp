/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:44:36 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/16 16:23:54 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#ifdef DEBUG
# define MLOG(str) std::cout << str << std::endl;
#else
# define MLOG(str) do {} while(false);
#endif

#ifndef MERROR 
# define MERROR(message) std::cout << "Error: " << message << std::endl; exit(1);
#endif

enum enum_server {
	LISTEN, 		//0
	SERVER_NAME, 	//1
	ROOT, 			//2
	INDEX,			//3
	AUTOINDEX,		//4
	ALLOW_METHODS,	//5
	CLIENT_MAX_SIZE,//6
	ERROR_PAGE		//7
};


// ========================
//  C++ Standard Libraries	
// ========================

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdlib> //stdlib deprecated
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
class Server;
std::vector<std::string> splitStringToVector(const std::string&);
std::vector<std::string> extractValues (const std::string&);
std::vector<std::string> vectorInitializer(const char**);
void printServer(std::vector<Server>&);


//	Templates
template <typename T>
void printMapVec(const std::map<T, std::vector<T> >& myMap) {
	//Using const_iterator instead of iterator because I don't intend to modify the elements of the container.
	for (typename std::map<T, std::vector<T> >::const_iterator it = myMap.begin(); it != myMap.end(); it++) {
		std::cout << "Key: [" << it->first << "] -> Values: {";
		for (typename std::vector<T>::const_iterator ut = it->second.begin(); ut != it->second.end(); ut++)
			std::cout << *ut << " ";
		std::cout << "}" << std::endl;
	}
}

template <typename T>
void printMapMapVec(const std::map<T, std::map<T, std::vector<T> > >& myMap) {
	std::cout << "printing MAP MAP:" << std::endl;
	for (typename std::map<T, std::map<T, std::vector<T> > >::const_iterator it = myMap.begin(); it != myMap.end(); it++) {
		std::cout << "map: " << it->first << std::endl;
		printMapVec(it->second);
		std::cout << std::endl << std::endl;
	}
}

#endif