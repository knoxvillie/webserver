/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/26 13:28:20 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

#include <sys/wait.h>

CgiHandler::CgiHandler(const t_request& request)
	: _request(request) {
	
	this->buildEnv();
	this->executeCgi();
}

CgiHandler::~CgiHandler() {}
 
/* void
CgiHandler::setEnvVariables(const std::map<std::string, std::string>& header) {
	std::map<std::string, std::string>::const_iterator it;
    for (it = header.begin(); it != header.end(); it++)
		*_envp = (it->first + "=" + it->second).c_str();
} */


// TODO: test this
const std::string
CgiHandler::getPathTranslated(void) {
	// Add root to the path_info
	std::string root_path;
	
	root_path = (_request.server->getBestLocation("/"))->root;
	
	return (root_path + _request.path_info);
}

void
CgiHandler::buildEnv()
{
	std::map<std::string, std::string> env;
	std::string uri = _request.url;

	env["AUTH_TYPE"] = ""; //Not used in our webserver, no authentication protocol needs to be implemented
	env["REQUEST_METHOD"] = _request.method;
	env["QUERY_STRING"] = _request.query_string; // call getQueryString before;
	env["REQUEST_URI"] = _request.url; // should this be unparsed_url ?
	env["SCRIPT_NAME"] = uri.substr(uri.rfind('/') + 1, uri.size());
	env["SERVER_PROTOCOL"] = ""; 
	env["CONTENT_TYPE"] = ""; // TODO
	env["CONTENT_LENGTH"] = "";  // TODO //in case of GET requests, no need to handle, get from POST requests
	env["PATH_INFO"] = _request.path_info;
	env["PATH_TRANSLATED"] = this->getPathTranslated();

	std::map<std::string, std::string>::const_iterator it;
    for (it = env.begin(); it != env.end(); it++)
		*_envp = (it->first + "=" + it->second).c_str();
}

void
CgiHandler::executeCgi() {

	int pipe_to_parent[2];
	int pipe_to_child[2];
	
	// Cant use pipe2, how do we make it nonblocking ?
	pipe(pipe_to_parent); // Write on pipe[1] read on pipe[0]
	pipe(pipe_to_child);

	// Might need to use fcntl for non blocking fds

	//_request.file_path = (_request.server->getBestLocation("/"))->root + _request.url;
	//_request.file_path = "WebServer/var/www/cgi-bin/helloworld.cgi";
	
	MLOG("EXECVE ARGS:\n");
	MLOG(_request.file_path.c_str());

	pid_t pid = fork();
	if (!pid) { // Child Process
		close(pipe_to_child[1]);
		dup2(pipe_to_child[0], STDIN_FILENO);
		close(pipe_to_child[0]);
		
		close(pipe_to_parent[0]);
		dup2(pipe_to_parent[1], STDOUT_FILENO);
		close(pipe_to_parent[1]);
		
		//char *argv[] = { const_cast<char*>((_request.url).c_str()), const_cast<char*>((_request.url).c_str()), NULL };
		// argv[0] is not reachable by execve when using filename in the first argument
		// but according to the subject: "Your program should call the CGI with the file requested as first argument."

		char *filename = const_cast<char *>("var/www/cgi-bin/helloworld.cgi");
		char *argv[] = {NULL, filename, NULL};
	

		// SCRIPT NEEDS TO HAVE EXEC PERMISSIONS
		if (execve(filename, argv, NULL) != 0) {
			MLOG("ERROR: execve() failed! errno = " << strerror(errno) << "\n"); // Kills the child process
			exit(1);
		}
	} else {
		close(pipe_to_child[0]);
		close(pipe_to_parent[1]);
		
		// TODO: sendToCgi()
		MLOG("\nCgi output: \n" + readFromCgi(pipe_to_parent[0]));
		// TODO: created response from Cgi output
	}
}

// TODO: test
void
CgiHandler::writeToCgi(int fd, const std::string& content) {
	if (write(fd, content.c_str(), content.size()) < 0)
		MLOG("ERROR: sendToCgi() failed.");
}

// TODO: test
std::string
CgiHandler::readFromCgi(int& fd) {
	char buf[BUFFER_SIZE] = {0};

	if (read(fd, buf, BUFFER_SIZE) < 0)
		throw std::runtime_error("Error: failed to read from cgi pipe.");
	return std::string(buf);
}
