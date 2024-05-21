/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/05/21 15:04:07 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler() {}
CgiHandler::~CgiHandler() {}
 
/* void
CgiHandler::setEnvVariables(const std::map<std::string, std::string>& header) {
	std::map<std::string, std::string>::const_iterator it;
    for (it = header.begin(); it != header.end(); it++)
		*_envp = (it->first + "=" + it->second).c_str();
} */


// TODO: test this
const std::string
CgiHandler::getPathTranslated(Request& request) {
	// Add root to the path_info
	std::string root_path;
	
	root_path = (request.server->getBestRedir("/"))->root;
	
	return (root_path + request.getPathInfo());
}

char **
CgiHandler::buildEnv(Request& request)
{
	std::map<std::string, std::string> env;
	std::string uri = request.getURI();

	env["AUTH_TYPE"] = ""; //Not used in our webserver, no authentication protocol needs to be implemented
	env["REQUEST_METHOD"] = request.getMethod();
	env["QUERY_STRING"] = request.getQueryString();
	env["REQUEST_URI"] = uri; // should this be unparsed_url ?
	env["SCRIPT_NAME"] = request.location->cgi_pass;
	env["SERVER_PROTOCOL"] = "";
	env["CONTENT_TYPE"] = request.getContentType();
	env["CONTENT_LENGTH"] = request.getContentLength();
	env["PATH_INFO"] = request.getPathInfo();
	env["PATH_TRANSLATED"] = CgiHandler::getPathTranslated(request);

	char** envp = new char*[env.size() + 1]; 

	std::map<std::string, std::string>::const_iterator it;
	int i = 0;
    for (it = env.begin(); it != env.end(); it++) {
		std::string line = (it->first + "=" + it->second).c_str();
		envp[i] = new char[line.size() + 1];
		std::strcpy(envp[i], line.c_str()); // strcpy also copies the terminating '\0'
		++i;
	}
	envp[i] = NULL;

	return envp;
}

Response*
CgiHandler::executeCgi(Request& request) {

	std::string script_name = request.location->cgi_pass;

	if (!isExecutable(script_name.c_str()))
		return (new Response(403, request.server)); // Might be 404

	int pipe_to_parent[2];
	int pipe_to_child[2];
	
	// Cant use pipe2, how do we make it nonblocking ?
	pipe(pipe_to_parent); // Write on pipe[1] read on pipe[0]
	pipe(pipe_to_child);
	// Might need to use fcntl for non blocking fds

	std::string response;

	MLOG("EXECVE ARGS:\n");
	MLOG(script_name);

	pid_t pid = fork();
	if (!pid) { // Child Process
		close(pipe_to_child[1]);
		dup2(pipe_to_child[0], STDIN_FILENO);
		close(pipe_to_child[0]);
		
		close(pipe_to_parent[0]);
		dup2(pipe_to_parent[1], STDOUT_FILENO);
		close(pipe_to_parent[1]);
		
		char* script = const_cast<char *>(script_name.c_str());
		char* filename = const_cast<char *>((request.getFilePath()).c_str());
		char* argv[] = {NULL, filename, NULL}; // TODO: might be script instead of filename in argv[1]
		// argv[0] is not reachable by execve when using filename in the first argument
		// but according to the subject: "Your program should call the CGI with the file requested as first argument."
		char** envp = CgiHandler::buildEnv(request);
		
		// SCRIPT NEEDS TO HAVE EXEC PERMISSIONS
		if (execve(script, argv, envp) != 0) {
			MLOG("ERROR: execve() failed! errno = " << strerror(errno) << "\n"); // Kills the child process
			free_env(envp);
			exit(1);
		}
		
	} else {
		close(pipe_to_child[0]);
		close(pipe_to_parent[1]);
		
		fcntl(pipe_to_child[1], F_SETFL, O_NONBLOCK);
		fcntl(pipe_to_parent[0], F_SETFL, O_NONBLOCK);

		request.cgi_pipes[0] = pipe_to_parent[0];
		request.cgi_pipes[1] = pipe_to_child[1];
		
/* 		CgiHandler::writeToCgi(pipe_to_child[1], request.getBody());
		close(pipe_to_child[1]); // This sends EOF to child (?)

		response = readFromCgi(pipe_to_parent[0]);
		MLOG("\nCgi output: \n" + response);
		// TODO: Test if we only send the response from the CGI when EOF is found
		close(pipe_to_parent[0]); */
	}
	//request.setToClose();
	request.cgi_finished = false;
	return (NULL);
}

// TODO: test
void
CgiHandler::writeToCgi(int fd, Request& request) {
	//if (write(fd, content.c_str(), content.size()) < 0)
		MLOG("ERROR: sendToCgi() failed.");
}

// TODO: test
void
CgiHandler::readFromCgi(int fd, Request& request) {
	GPS;
	char buf[BUFFER_SIZE] = {0};
	int bytes;

	memset(buf, 0, BUFFER_SIZE);
	bytes = read(fd, buf, BUFFER_SIZE);
	MLOG("bytes: " << bytes);
	
	if (bytes < 0)
		return;

	if (bytes == 0) { // this indicates that the cgi has closed the pipe
		close(fd);
		request.cgi_finished = true;
		return;
	}

	(request.cgiBuf).append(std::string(buf));
	// The size of the body of the response
	request.cgi_bytes = (request.cgiBuf).substr((request.cgiBuf).find("\r\n\r\n") + 4).size();

	int content_length = -1, pos = (request.cgiBuf).find("Content-length: ");
	
	if (pos != std::string::npos) {
		std::string line = (request.cgiBuf).substr(0, (request.cgiBuf).find("\r\n", pos)); // everything until the end of the line containing "Content-length: "
		content_length = std::atoi(line.substr(pos + 17).c_str());
	}

	if ((content_length != -1) && request.cgi_bytes >= content_length) {
		close(fd);
		request.cgi_finished = true;
		return;
	}
}
