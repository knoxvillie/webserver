/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/12 14:39:59 by diogmart         ###   ########.fr       */
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

/* CgiHandler::CgiHandler(const std::string& file_name, const std::string& extension)
	: file(file_name), extention(extension) {} */

CgiHandler::CgiHandler(const t_request& request) : _request(request) {
	std::string file = this->_request.file_path;
	_extention = file.substr(file.find_last_of('.'), std::string::npos);

	try {
		_interpreter = (extentionToInterpreter.at(_extention)).c_str();
	} catch (std::out_of_range& e) { // if at() doesnt exist in map
		MLOG("NO CGI FOUND FOR THAT EXTENTION!");
		_interpreter = NULL;
	}
}

CgiHandler::~CgiHandler() {}
 
void
CgiHandler::setEnvVariables(const std::string& name, const std::string& value) {
	*_envp = (name + "=" + value).c_str();
}

void
CgiHandler::executeCgi() {

	if (_interpreter == NULL)
		return;

	// fork()

	// dup2 and what not

	// execve(_interpreter, _request.file_path, _envp)
}

