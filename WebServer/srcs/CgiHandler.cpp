/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:26 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/23 17:04:37 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiHandler.hpp"

CgiHandler::CgiHandler(const t_request& request)
	: _request(request), QUERY_STRING(request.query_string), PATH_INFO(request.path_info) {
	
	std::string file = this->_request.file_path;
	_extension = file.substr(file.find_last_of('.'), std::string::npos);
	this->setPathTranslated();
	
	// To handle multiple CGIs we just need to execute scripts ending in .cgi as long as they start with #!/interpreter
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

	int pipe_to_parent[2];
	int pipe_to_child[2];
	
	// Cant use pipe2, how do we make it nonblocking ?
	pipe(pipe_to_parent); // Write on pipe[1] read on pipe[0]
	pipe(pipe_to_child);

	pid_t pid = fork();
	if (!pid) { // Child Process
		close(pipe_to_child[1]);
		dup2(pipe_to_child[0], STDIN_FILENO);
		close(pipe_to_child[0]);
		
		close(pipe_to_parent[0]);
		dup2(pipe_to_parent[1], STDOUT_FILENO);
		close(pipe_to_parent[1]);
		
		char *argv[] = { NULL, const_cast<char*>((_request.file_path).c_str()), NULL };
		// argv[0] is not reachable by execve when using filename in the first argument
		
		// TODO: execve
		// According to the subject: Your program should call the CGI with the file requested as first argument.
		execve((_request.file_path).c_str(), argv, const_cast<char**>(_envp)); // SCRIPT NEEDS TO HAVE EXEC PERMISSIONS
	
	} else {
		close(pipe_to_child[0]);
		close(pipe_to_parent[1]);
		
		//TODO: send(pipe_to_child[1], ...)
		//TODO: recv(pipe_to_parent[0], ...) and whatever
	}
}
