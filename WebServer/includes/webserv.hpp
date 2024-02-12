/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/02 11:44:36 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/12 13:03:55 by kfaustin         ###   ########.fr       */
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
bool isTokenInDirectives(const std::string& token, const std::string&);
std::vector<std::string> extractValues (const std::string& input);
std::vector<std::string> vectorInitializer(const char **list);

//	Templates
template <typename T>
void printMapVec(const std::map<T, std::vector<T> >& myMap) {
	//Using const_iterator instead of iterator because I don't intend to modify the elements of the container.
	std::cout << "printing MAP:" << std::endl;
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
		std::cout << it->first << ">>>>>>>>>>>"<< std::endl;
		printMapVec(it->second);
		std::cout << ">>>>>>>>>>>" << std::endl;
	}
}

#endif