/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/18 12:03:35 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

std::map<std::string, std::string> CgiHandler::extensionToInterpreter;

void 
CgiHandler::initMap(void) {
	// Supported scripting languages:
	extensionToInterpreter[".py"] = "/usr/bin/python3";
	extensionToInterpreter[".php"] = "/usr/bin/php";		
	extensionToInterpreter[".pl"] = "/usr/bin/perl";
}

CgiHandler::CgiHandler(const t_request& request) : _request(request) {
	std::string file = this->_request.file_path;
	_extension = file.substr(file.find_last_of('.'), std::string::npos);

	try {
		_interpreter = (extensionToInterpreter.at(_extension)).c_str();
	} catch (std::out_of_range& e) { // if at() doesnt exist in map
		MLOG("NO CGI FOUND FOR THAT extension!");
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


// TODO: Check if this is still needed
void
CgiHandler::getQueryString(void) {
	std::string url = _request.unparsed_url;

	if (url.find("?"))
		QUERY_STRING = url.substr(url.find("?") + 1);
}

// TODO: Check if this is still needed
// Path info might need to be root + request.path_info
void
CgiHandler::getPathInfo(void) {
	std::string url = _request.url;
	int len = _extension.length();
	size_t pos, i;

	i = 0;
	do {
		pos = url.find(_extension, i);
		i = pos;
		std::string current_file = url.substr(url.find("/", pos)); 
		/* 
		 Should it be len here ? or check until the next "/"?
		 it will cause errors if there is a dir that contains _extension
		 but doesn't end with it. (e.g. thisisa.pydirectory)
		*/
		if (isDirectory(current_file))
			continue;
		else {
			PATH_INFO = url.substr(pos + len); // Could be std::string::npos
			break;
		}

	} while (pos != std::string::npos && (pos != (url.length() - len)));
}


std::map<std::string, std::string> CgiHandler::buildEnv() 
{
	std::map<std::string, std::string> env;
	std::string uri = this->_request.url;

	env["AUTH_TYPE"] = ""; //Not used in our webserver, no authentication protocol needs to be implemented
	env["REQUEST_METHOD"] = this->_request.method;
	env["QUERY_STRING"] = this->QUERY_STRING; // call getQueryString before;
	env["REQUEST_URI"] = this->_request.url; // should this be unparsed_url ?
	env["SCRIPT_NAME"] = uri.substr(uri.rfind('/') + 1, uri.size());
	env["SERVER_PROTOCOL"] = "";
	env["CONTENT_TYPE"] = "";
	env["CONTENT_LENGTH"] = ""; //in case of GET requests, no need to handle, get from POST requests
	// TODO: ADD PATH_TRANLATED

	return (env);
}

void
CgiHandler::executeCgi() {

	if (_interpreter == NULL)
		return;

	// pid_t = fork()

	// dup2 and what not

	// execve(_interpreter, _request.file_path, _envp)
}

