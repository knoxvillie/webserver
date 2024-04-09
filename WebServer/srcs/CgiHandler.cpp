/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/09 14:55:53 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

std::map<std::string, std::string> CgiHandler::extentionToInterpreter;

void CgiHandler::initMap(void) {
	// Supported scripting languages:
	extentionToInterpreter[".py"] = "/usr/bin/python3";
	extentionToInterpreter[".php"] = "/usr/bin/php";		
	extentionToInterpreter[".pl"] = "/usr/bin/perl";
}

CgiHandler::CgiHandler(const std::string& file_name, const std::string& extension)
	: file(file_name), extention(extension) {}

CgiHandler::~CgiHandler() {}

