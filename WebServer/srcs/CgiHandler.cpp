/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/22 14:27:41 by diogmart         ###   ########.fr       */
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
 
/* void
CgiHandler::setEnvVariables(const std::map<std::string, std::string>& header) {
	std::map<std::string, std::string>::const_iterator it;
    for (it = header.begin(); it != header.end(); it++)
		*_envp = (it->first + "=" + it->second).c_str();
}
 */

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

	int pipefds[2];
	int tmp;
	
	// Cant use pipe2, how do we make it nonblocking ?
	pipe(pipefds); // Write on pipefds[1] read on pipefds[0]
	
	//TODO: Might need to create another pipe to send the request body to the script in the STDIN

	pid_t pid = fork();
	if (!pid) { // Child Process
		dup2(pipefds[1], STDOUT_FILENO);
		close(pipefds[0]);
		close(pipefds[1]);
		
		//TODO: execve
		// According to the subject: Your program should call the CGI with the file requested as first argument.
		//execve((_request.file_path).c_str(), NULL, _envp);
	
	} else {
		close(pipefds[1]);
		//TODO: recv(pipefds[0], ...) and whatever
	}
}

