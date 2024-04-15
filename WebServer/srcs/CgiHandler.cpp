/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/15 14:56:51 by diogmart         ###   ########.fr       */
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
	_extension = file.substr(file.find_last_of('.'), std::string::npos);

	try {
		_interpreter = (extentionToInterpreter.at(_extension)).c_str();
	} catch (std::out_of_range& e) { // if at() doesnt exist in map
		MLOG("NO CGI FOUND FOR THAT EXTENTION!");
		_interpreter = NULL;
	}
}

CgiHandler::~CgiHandler() {}
 
void
CgiHandler::setEnvVariables(const std::map<std::string, std::string>& header) {
	std::map<std::string, std::string>::const_iterator it;
    for (it = header.begin(); it != header.end(); it++)
		*_envp = (it->first + "=" + it->second).c_str();
}

void
CgiHandler::getQueryString(void) {
	std::string url = _request.url;

	if (url.find("?"));
		QUERY_STRING = url.substr(url.find("?") + 1);
}

// THIS IS WRONG, should be filename not extension
void
CgiHandler::getPathInfo(void) {
	std::string url = _request.url;
	size_t pos = url.find(_extension);
	int len = _extension.length();
	
	if (pos != std::string::npos && (pos != (url.length() - len)))
		PATH_INFO = url.substr(pos + len);
}

void
CgiHandler::executeCgi() {

	if (_interpreter == NULL)
		return;

	// fork()

	// dup2 and what not

	// execve(_interpreter, _request.file_path, _envp)
}

