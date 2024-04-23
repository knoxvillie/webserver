/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 18:34:53 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/23 15:35:08 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Http.hpp"

// Prototypes
static void generateResponse(std::ostringstream&, const std::string&, const std::string&, const std::string&);


Http::Http(int connection, Server* server) : _client(connection), _server(server) {
	(this->request).server = server;
	this->requestFromClient();
	this->setHeaderAndBody();
	this->requestParser();
	this->responseSend();
}

Http::~Http() {}

void Http::requestFromClient() {
	GPS;
	char content[BUFFER_SIZE] = {0};

	// Multiple recv calls might be needed for a request bigger than BUFFER_SIZE no ?
	// also BUFFER_SIZE value was just a placeholder, might need to be changed
	if (recv(this->_client, content, BUFFER_SIZE, MSG_DONTWAIT) < 0)
		throw std::runtime_error("Error: Read from client socket");
	(this->request).full = std::string(content);
	MLOG(content);
}

void Http::requestParser(void) {
	GPS;
	std::string token;
	std::stringstream ss((this->request).full);

	if (ss >> token) {
		if (token != "GET" && token != "POST" && token != "DELETE")
			throw std::runtime_error("Error: Invalid HTTP request method");
		(this->request).method = token;
	} else throw std::runtime_error("Error: Can't read the method in the HTTP request line");

	if (ss >> token) {
		(this->request).unparsed_url = token;
		this->ParseURL();
	} else throw std::runtime_error("Error: Can't read URI in HTTP request line");

	if (ss >> token) {
		if (token != "HTTP/1.1")
			throw std::runtime_error("Error: Invalid HTTP version");
		this->http_version = token;
	} else throw std::runtime_error("Error: Can't read the version in HTTP request line");
}

static void
generateResponse(std::ostringstream& oss, const std::string& http_version, const std::string& status_code, const std::string& content) {
	oss << http_version << " " << status_code << "\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << content.length() << "\r\n";
	oss << "\r\n";
	oss << content;
}

void
Http::generateErrorResponse(std::ostringstream& oss, int error_code) {
	// Find the error page corresponding to the error code
	std::map<int, std::string>::const_iterator error_page_it;
	error_page_it = this->_server->getErrorMap().find(error_code);

	// There is an error page defined for the specif error
	if (error_page_it != this->_server->getErrorMap().end()) {
		std::string error_path = this->_server->getErrorMap()[error_code];
		std::string path(this->_server->getPWD() + error_path);
		path = path.substr(0, path.find(';'));

		// Open the error page
		std::ifstream file(path.c_str());
		if (file.is_open()) {
			// Read the content of the error page
			std::stringstream buffer;
			buffer << file.rdbuf();
			std::string content = buffer.str();
			file.close();

			// Generate the HTTP response with the content of the error page
			generateResponse(oss, this->http_version, intToString(error_code) + " Not Found", content);
		} else {
			throw std::runtime_error("Error: Cannot open the error_page file");
		}
	} else {
		throw std::runtime_error("Error: Error page not found for code " + intToString(error_code));
	}
}

/*
	Things to change in the generateErrorResponse() function:
		- Dont throw exceptions just because one server couldn't find an error page, probably just send a standard error
		to the client and move on;
		- Not all errors will be "Not Found", only 404 is. Read https://datatracker.ietf.org/doc/html/rfc2616#autoid-45 for more info
*/

#include <sys/stat.h>
#include <ctime>

std::string dirTypes(unsigned char type) {
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

std::string formatSize(size_t size) {
	std::stringstream sstream;
	if (size < 1024) sstream << size << " B";
	else if (size < 1024 * 1024) sstream << size / 1024 << " KB";
	else if (size < 1024 * 1024 * 1024) sstream << size / (1024 * 1024) << " MB";
	else sstream << size / (1024 * 1024 * 1024) << " GB";
	return (sstream.str());
}

std::string Http::directoryListing(void) {
	DIR* dir;
	struct dirent* entry;
	struct stat file_stat;
	std::stringstream html;

	MLOG("PATH-> " + this->request.file_path);
	dir = opendir(this->request.file_path.c_str());
	if (!dir) {
		MLOG("ERROR: Unable to open directory");
		return "<html><head><title>Error</title></head><body><h1>Error opening directory.</h1></body></html>";
	}
	html << "<html><head><title>Directory Listing</title>"
		<< "<style>"
		<< "body { font-family: Arial, sans-serif; margin: 20px; }"
		<< "table { width: 100%; border-collapse: collapse; }"
		<< "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }"
		<< "th { background-color: #f2f2f2; }"
		<< "h2 {"
		<< "  color: #333;"
		<< "  text-align: center;"
		<< "  font-size: 24px;"
		<< "  font-weight: normal;"
		<< "  text-shadow: 1px 1px 1px #aaa;"
		<< "  margin-bottom: 20px;"
		<< "  padding: 10px 0;"
		<< "  border-bottom: 2px solid #eee;"
		<< "}"
		<< "</style>"
		<< "</head><body>"
		<< "<h2>Listing of " << this->request.file_path << "</h2>"
		<< "<table><tr><th>Filename</th><th>Type</th><th>Creation Date</th><th>Size</th></tr>";

	while ((entry = readdir(dir))) {
		if (std::strcmp(entry->d_name, ".") == 0 ) continue; // Skip hidden current directory markers.
		std::string filePath = this->request.file_path + "/" + entry->d_name;
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

void
Http::responseSend(void) {
	int statusCode;
	struct stat buf;
	std::string content;
	std::ostringstream oss;
	t_location* actual_location;

	// Find the location corresponding to the URL
	statusCode = 404;
	actual_location = this->_server->getBestLocation(request.url);

	// If the location is found in the URL
	if (actual_location != NULL) {
		request.file_path = actual_location->root + request.url;

		// It is a location or a directory
		if (request.file_path[request.file_path.size() - 1] == '/') {
			// Check if the location index exists
			if (stat(actual_location->index.c_str(), &buf) == 0) {
				request.file_path = actual_location->index;
				statusCode = getMethod(actual_location->allow_methods, content);
			}
			// If auto_index off or the directory doesn't exist and the index file cannot be opened
			else if (!actual_location->auto_index || (stat(request.file_path.c_str(), &buf) != 0)) {
				generateErrorResponse(oss, 403); // Forbidden request
				return;
			}
			else
				generateResponse(oss, http_version, "200", directoryListing());
		} // file_path must be a file, and it will be checked on the response.

		else if (request.method == "GET")
			statusCode = getMethod(actual_location->allow_methods, content);
		else if (request.method == "POST")
			statusCode = postMethod(actual_location->allow_methods);


		switch (statusCode) {
			case 200:
				generateResponse(oss, http_version, "200 OK", content);
				break;
			case 201:
				generateResponse(oss, http_version, "201 Created", content);
				break;
			default:
				generateErrorResponse(oss, statusCode);
				break;
		}
	} else {
		MLOG("LOCATION NOT FOUND");
		// Location not found, generate a 404 Not Found response
		generateErrorResponse(oss, 404);
	}

	// Get the generated response
	std::string response = oss.str();

	// Send the response to the client
	if (send(this->_client, response.c_str(), response.length(), 0) < 0) {
		throw std::runtime_error("Error: send function failed");
	}
}

int
Http::getMethod(const std::vector<std::string>& methods, std::string& content) {
	// Method not allowed on location
	if (std::find(methods.begin(), methods.end(), "GET") == methods.end())
		return 405;
	std::stringstream buffer;
	std::ifstream file((request.file_path).c_str());

	MLOG("~~~~~~~~\n   GET\n~~~~~~~~");
	if (!file) {
		// No such file or directory
		if (errno == ENOENT)
			return (404);
		// Permission denied
		else if (errno == EACCES)
			return (403);
		// Internal Server Error
		else
			return (500);
	}
	buffer << file.rdbuf();
	content = buffer.str();
	file.close();
	// Success
	return (200);
}

int
Http::postMethod(const std::vector<std::string>& methods) {
	std::ofstream out_file(request.file_path.c_str(), std::ofstream::app);

	// Status code for method not allowed
	if (std::find(methods.begin(), methods.end(), "POST") == methods.end())
		return (405);

	MLOG("~~~~~~~~\n   POST\n~~~~~~~~");


	if (!out_file) {
		// No such file or directory
		if (errno == ENOENT)
			return (404);
			// Permission denied
		else if (errno == EACCES)
			return (403);
			// Internal Server Error
		else
			return (500);
	}
	std::string output = this->request.full.substr(this->request.full.find("\r\n\r\n") + 4, std::string::npos);

	MLOG("Output: " + output);
	out_file << "\n**************\n\n";
	out_file << output;
	out_file << std::endl;
	out_file.close();

	// Generate the HTTP 200 OK response
	return (201);
}

int
Http::deleteMethod(const t_location *location) {
	MLOG("~~~~~~~~\n   DEL\n~~~~~~~~");

	if (std::find(location->allow_methods.begin(), location->allow_methods.end(), "DELETE") == location->allow_methods.end())
		return 405; // Status code for method not allowed

	(void)location;
	return 200;
}

void
Http::setHeaderAndBody(void) {
	GPS;
	t_location *actual_location;
	std::string content = request.full;

	this->request.first_line = content.substr(0, content.find("\r\n"));
	this->request.header = content.substr((request.first_line).length() + 2, content.find("\r\n\r\n"));
	this->request.body = content.substr(content.find("\r\n\r\n") + 1, std::string::npos);
	actual_location = this->_server->getBestLocation(this->request.url);

	//if (actual_location->CMaxBodySize * 1024 * 1000 < this->request.body.size())


	fillHeaderMap();
}

void
Http::fillHeaderMap(void) {
	GPS;
	std::string line, header = request.header;
	size_t pos = 0;

	MLOG("\n\n\n\nHEADER: " + header);

	while ((pos = header.find("\r\n")) != std::string::npos) {

		line = header.substr(0, pos);
		header.erase(0, pos + 2);
		if (line.empty())
			break;

		size_t colPos = line.find(":");
		if (colPos != std::string::npos) {
			std::string key = line.substr(0, colPos);
			std::string value = line.substr(colPos + 2, std::string::npos); // +2 because of whitespace after ":"
			request.headerMap[key] = value;
		}
	}

	// printing map for debug
	std::map<std::string, std::string>::const_iterator it;
	for (it = request.headerMap.begin(); it != request.headerMap.end(); it++) {
		std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
	}
}

void
Http::ParseURL(void)
{
	GPS;
	MLOG("\n\nUNPARSED URL: " + request.unparsed_url + "\n\n");
	std::string extension, url = request.unparsed_url;
	size_t pos;

	pos = url.find(".");
	if (pos == std::string::npos) {
		request.path_info = "/";
		if ((pos = url.find("?")) && pos != std::string::npos) { // if there is a query_string it will be ignored but store it anyway
			request.query_string = url.substr(pos + 1);
			request.url = url.substr(0, pos);
		} else
			request.url = url;
		MLOG("PARSED URL: " + request.url + "\n\n");
		MLOG("QUERY STRING: " + request.query_string + "\n\n");
		return;
	} else
		extension = url.substr(pos);

	pos = extension.find("?"); // There is a query_string
	if (pos != std::string::npos) {
		request.query_string = extension.substr(pos + 1);
		extension = extension.substr(0, pos);

		MLOG("QUERY STRING: " + request.query_string + "\n\n");
		
	}
	pos = extension.find("/");
	if (pos != std::string::npos) { // There is path_info
		request.path_info = extension.substr(pos); // no need for extension.find("?") because we already remove query string before
		extension = extension.substr(0, pos);

		MLOG("PATH INFO: " + request.path_info + "\n\n");
	}
	
	if (extension != ".cgi") request.isCGI = false;
	else request.isCGI = true;

	MLOG("CGI: " << request.isCGI << "\n\n");
	
	request.url = url.substr(0, (url.find(extension) + extension.length()));
	MLOG("PARSED URL: " + request.url + "\n\n");
}

void 
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
}
