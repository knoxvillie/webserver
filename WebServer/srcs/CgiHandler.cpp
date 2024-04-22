/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/22 12:04:28 by diogmart         ###   ########.fr       */
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

CgiHandler::CgiHandler(const t_request& request)
	: _request(request), QUERY_STRING(request.query_string), PATH_INFO(request.path_info) {
	
	std::string file = this->_request.file_path;
	_extension = file.substr(file.find_last_of('.'), std::string::npos);
	this->setPathTranslated();

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


// TODO: test this
void
CgiHandler::setPathTranslated(void) {
	// Add root to the path_info
	std::string root_path;
	
	root_path = ((this->_request).server->getBestLocation("/"))->root;
	
	PATH_TRANSLATED = root_path + PATH_INFO;
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
	env["PATH_INFO"] = this->PATH_INFO;
	env["PATH_TRANSLATED"] = this->PATH_TRANSLATED;

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

