/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 18:34:53 by kfaustin          #+#    #+#             */
/*   Updated: 2024/05/29 10:20:20 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"
#include "Response.hpp"
#include "utils.hpp"
#include "webserv.hpp"

// Prototypes
static std::string dirTypes(const unsigned char&);
static std::string formatSize(const size_t&);

Http::Http() {}

Http::~Http() {}

/* 
NOTE:
 Recv doesn't need to be called in a loop if handled properly, as epoll_wait will keep
 returning EPOLLIN in the socket as long as there is more info to read. We just need
 to make sure all the info is received before we handle the connection
*/

void
Http::receiveFromClient(int socket, Request& request) {
	GPS;
	char buf[BUFFER_SIZE] = {0};
	int bytes;

	memset(buf, 0, BUFFER_SIZE);
	bytes = recv(socket, buf, BUFFER_SIZE, 0);
	MLOG("bytes: " << bytes);
		
	request.receiveData(std::string(buf), bytes);
	if (bytes == 0)
		throw Http::HttpConnectionException("Error - Connection closed before request was finished");
	if (bytes < 0)
		return;
}

void
Http::requestParser(Request& request) {
	GPS;
	std::string token;
	std::stringstream ss(request.getFull());

	if (request.getFull().empty())
		return;

	MLOG("STREAM: " << ss.str());
	if (ss >> token) {
		if (token != "GET" && token != "POST" && token != "DELETE")
			throw Http::HttpErrorException(405);
		request.setMethod(token);
	} else throw Http::HttpErrorException(400);

	if (ss >> token) {
		request.setURI(token); // Parses the URI and sets it
	} else throw Http::HttpErrorException(400);

	if (ss >> token) {
		if (token != "HTTP/1.1")
			throw Http::HttpErrorException(505);
		request.setHttpVersion(token);
	} else throw Http::HttpErrorException(400);
}

Response*
Http::BuildResponse(Request& request) {
	GPS;
	bool is_redirect = false;
	t_location* best_location;
	
	Http::requestParser(request);
	request.setBody();
	MLOG("Request: " << request.getFull());
	
	// Find the location corresponding to the URL
	best_location = request.server->getBestLocation(request);
	request.location = best_location;
	// Location not found, generate a 404 Not Found response
	if (best_location == NULL)
		return (new Response(404, request.server));
	// Checking Location Client Max Body Size.
	else if (size_t(best_location->CMaxBodySize) < (request.getBody()).size())
		return (new Response(403, request.server));
	// Handle Cgi Requests
	else if (request.isCGI())
		return (CgiHandler::executeCgi(request));

	std::string URI = request.getURI(), loc_name = best_location->location_name;
	std::string relative_filepath = URI.substr(URI.find(loc_name) + (loc_name.size() - 1)); // -1 because of '/'
	request.setFilePath(best_location->root + relative_filepath);
	
	// Handle redirect
	if (best_location->redirect != "false") {
		is_redirect = true;
		request.setToClose();
		if (best_location->redirect_is_extern)
			return (new Response(302, best_location->redirect, "text/html"));
		else {
			best_location = request.server->getBestRedir(best_location->redirect);
			// Location not found to redirect.
			if (best_location == NULL)
				return (new Response(404, request.server));
			request.setFilePath(best_location->location_name);
			return (new Response(302, request.getFilePath()));
		}
	}
	else if (Utils::isDirectory(request.getFilePath()))
		return (Http::doDirectoryResponse(request, is_redirect));
	else
		return (Http::handleMethod(request));
}

Response*
Http::handleMethod(Request& request) {
	GPS;
	int status_code = 501;
	t_location* location = request.location;
	std::string content, type;

	MLOG("METHOD: " << request.getMethod());

	// The url is requesting a file
	if (request.getMethod() == "GET")
		status_code = Http::getMethod(request.getFilePath(), location->allow_methods, content);
	else if (request.getMethod() == "POST") {
		/* if (((request.getHeaderMap().find("Content-type"))->second).find("multipart/form-data") != std::string::npos)
			status_code = Http::handleUpload(request);	
		else */
		status_code = Http::postMethod(request.getFilePath(), location->allow_methods, request.getBody());
	}
	else if (request.getMethod() == "DELETE")
		status_code = Http::deleteMethod(request.getFilePath(), location->allow_methods);

	// TODO: make a type file getter
	if ((request.getFilePath()).find(".css") != std::string::npos)
		type = "text/css";
	else
		type = "text/html";

	return (new Response(status_code, content, type));
}

Response*
Http::doDirectoryResponse(Request& request, bool is_redirect) {
	struct stat buf;
	t_location* location = request.location;
	std::string content;

	// Check if the location index exists
	if (stat(location->index.c_str(), &buf) == 0) {
		// Index exists so must be sent.
		request.setFilePath(location->index);
		return Http::handleMethod(request);
	}
	// If auto_index off and the index doesn't exist -> forbidden request
	else if (!location->auto_index)
		throw Http::HttpErrorException(403);
	// then listing
	else {
		if (is_redirect)
			return (new Response(302, Http::directoryListing(request), "text/html"));
		else
			return (new Response(200, Http::directoryListing(request), "text/html"));
	}
}

/*
	Things to change in the findErrorPage() function:
		- Dont throw exceptions just because one server couldn't find an error page, probably just send a standard error
		to the client and move on;
		- Not all errors will be "Not Found", only 404 is. Read https://datatracker.ietf.org/doc/html/rfc2616#autoid-45 for more info
*/

std::string
Http::directoryListing(Request& request) {
	DIR* dir;
	struct dirent* entry;
	struct stat file_stat;
	std::stringstream html;

	dir = opendir((request.getFilePath()).c_str());
	if (!dir) {
		MLOG("ERROR: Unable to open directory");
		return "<html><head><title>Error</title></head><body><h1>Error opening directory.</h1></body></html>";
	}
	Utils::createListingPage(html, request.getFilePath());
	while ((entry = readdir(dir))) {
		if (std::strcmp(entry->d_name, ".") == 0 ) continue; // Skip hidden current directory markers.
		std::string filePath = request.getFilePath() + "/" + entry->d_name;
		if(stat(filePath.c_str(), &file_stat) == 0) {
			char time_buf[100];
			std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_ctime));
			html << "<tr><td><a href='" << ((entry->d_type == DT_DIR) ? (std::string(entry->d_name) + "/") : entry->d_name) << "'>" << entry->d_name << "</a></td>"
				 << "<td>" << dirTypes(entry->d_type) << "</td>"
				 << "<td>" << time_buf << "</td>"
				 << "<td>" << formatSize(file_stat.st_size) << "</td></tr>";
		}
	}
	closedir(dir);
	html << "</table></body></html>";
	return html.str();
}

int
Http::getMethod(const std::string& file_path, const std::vector<std::string>& methods, std::string& content) {
	MLOG("~~~~~~~~\n   GET\n~~~~~~~~");
	
	if (std::find(methods.begin(), methods.end(), "GET") == methods.end())
		throw Http::HttpErrorException(405);

	std::stringstream buffer;
	int flag = Utils::isRegularFile(file_path);
	
	if (flag == 1) {
		std::ifstream file(file_path.c_str());
		if (!file.is_open())
			throw Http::HttpErrorException(500);
		buffer << file.rdbuf();
		content = buffer.str();
		file.close();
		return (200);
	}
	// The file doesn't exist.
	else if (flag == -1) throw Http::HttpErrorException(404);
	// The request is a directory, special device or a symbolic link.
	else throw Http::HttpErrorException(403);
	
	return (400);
}

int
Http::postMethod(const std::string& file_path, const std::vector<std::string>& methods, const std::string& body) {
	MLOG("~~~~~~~~\n   POST\n~~~~~~~~");
	
	// Status code for method not allowed
	if (std::find(methods.begin(), methods.end(), "POST") == methods.end())
		throw Http::HttpErrorException(405);

	int flag = Utils::isRegularFile(file_path);
	if (flag == 0)
		throw Http::HttpErrorException(400);

	int statusCode = (flag == -1) ? 201 : 200;

	std::ofstream out_file(file_path.c_str(), std::ofstream::app); // Open file in append mode, create one if it doesn't exist

	if (!out_file) {
		// No such file or directory
		if (errno == ENOENT)
			throw Http::HttpErrorException(404); // This should never get here because it will create a file if it doesn't exit already
		// Permission denied
		else if (errno == EACCES)
			throw Http::HttpErrorException(403);
		// Internal Server Error
		else
			throw Http::HttpErrorException(500);
	}

	const std::string output = body;
	if (output.empty())
		return 204;
	MLOG("Output: " + output);
	
	out_file.write(output.c_str(), output.size());
	out_file.write("\n", 1);	
	out_file.close();

	// Generate the HTTP 201 OK response
	return (statusCode);
}

int
Http::deleteMethod(const std::string& file_path, const std::vector<std::string>& methods) {
	MLOG("~~~~~~~~\n   DELETE\n~~~~~~~~");

	if (std::find(methods.begin(), methods.end(), "DELETE") == methods.end())
		throw Http::HttpErrorException(405);

	int flag = Utils::isRegularFile(file_path);
	if (flag == 0)
		throw Http::HttpErrorException(400);
	
	if (std::remove(file_path.c_str()) != 0) {
        throw Http::HttpErrorException(500);
    }
	
	return (200);
}

int
Http::handleUpload(const Request& request) {
	MLOG("~~~~~~~~\n   UPLOAD\n~~~~~~~~");
	(void)request;
	return 0;
/* 	std::string content_type, boundary, body, part, line;

	content_type = (request.getHeaderMap().find("Content-type"))->second;
	boundary = content_type.substr(content_type.find("bondary=") + 9);
	boundary = boundary.substr(0, boundary.find(";"));

	MLOG(".|. Boundary: " << boundary << " .|.\n");

	body = request.getBody();
	for (int i = 0; i < body.size();) {
		// Get each part
		int partStart = body.find(boundary, i) + boundary.size();	
		int partEnd = body.find(boundary, partStart);
		part = body.substr(partStart, partEnd - partStart); // Note: might need to add 1
		i = partEnd;	// Place i at the end of the part found
		
		// Separate header and body of the current part
		std::string partHeader = part.substr(0, part.find("\r\n\r\n"));
		std::string partBody = part.substr(part.find("\r\n\r\n") + 4);
		
		// Get the filename from the header
		int pos = partHeader.find("filename=") + 10; // 10 because 9 is for filename=, and 1 extra for the " after
		std::string filename = (pos != std::string::npos) ? partHeader.substr(pos, partHeader.find("\"", pos)) : "default";
		
		// Create the file and send the body there
		 
	}*/
}


static std::string
dirTypes(const unsigned char& type) {
	switch (type) {
		case DT_BLK: return ("block device");
		case DT_CHR: return ("character device");
		case DT_DIR: return ("directory");
		case DT_FIFO: return ("named pipe (FIFO");
		case DT_LNK: return ("symbolic link");
		case DT_REG: return ("regular file");
		case DT_SOCK: return ("UNIX domain socket");
		default: return ("unknown");
	}
}

static std::string
formatSize(const size_t& size) {
	std::stringstream sstream;
	if (size < 1024) sstream << size << " B";
	else if (size < 1024 * 1024) sstream << size / 1024 << " KB";
	else if (size < 1024 * 1024 * 1024) sstream << size / (1024 * 1024) << " MB";
	else sstream << size / (1024 * 1024 * 1024) << " GB";
	return (sstream.str());
}

/* void 
Http::decodeURI() {
  
	std::string	newUri;

	for (std::string::iterator iter = this->_decodedURI.begin(); iter != this->_decodedURI.end(); ++iter) 
	{
		if (*iter == '%' && iter + 1 != this->_decodedURI.end() && iter + 2 != this->_decodedURI.end())
		{
			char numberChar[3] = {*(iter + 1), *(iter + 2), 0};
			char *endptr = NULL;
			u_int64_t numberInt = std::strtoul(numberChar, &endptr, 16);
			if (endptr && *endptr == 0)
			{
				newUri += static_cast<char>(numberInt);
				iter += 2;
			}
		} else {
			newUri += *iter;
		}
	}
	this->request.url = newUri;
} */

Http::HttpErrorException::HttpErrorException() : http_error(400), message("Bad Request") {}

Http::HttpErrorException::HttpErrorException(int error) : http_error(error), msg(Response::getStatusMessage(error)), message(msg.c_str()) {}

Http::HttpErrorException::~HttpErrorException() throw() {}

const char*
Http::HttpErrorException::what() const throw() {
	return message;
}

int
Http::HttpErrorException::getErrorCode() const {
        return this->http_error;
}


Http::HttpConnectionException::HttpConnectionException(std::string msg) : message(msg.c_str()) {}
Http::HttpConnectionException::~HttpConnectionException() throw() {}
const char* Http::HttpConnectionException::what() const throw() {
	return message;
}
	