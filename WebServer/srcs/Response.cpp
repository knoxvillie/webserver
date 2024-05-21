/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:02:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/05/20 14:35:36 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response() {}

Response::Response(const std::string& cgi_content) : cgi_response(cgi_content) {}

Response::Response(int statusCode, const std::string& content) : status_code(statusCode), body(content) {
	buildHeaderMap();
	buildHeader();
}

Response::Response(int statusCode, const std::string& content, const std::string& type) : status_code(statusCode), body(content), contentType(type) {
	buildHeaderMap();
	buildHeader();
}

Response::Response(int errorCode, Server* arg) : status_code(errorCode), server(arg) {
	this->contentType = "text/html";
	
	if (!findErrorPage(errorCode)) {
		Utils::createStyleIfNotExists();
		this->body = Utils::createGenericErrorPage(this->status_code, Response::getStatusMessage(this->status_code));
	}
	buildHeaderMap();
	buildHeader();
}

Response::~Response() {}

void
Response::buildHeader() {
	std::stringstream buf;

	buf << "HTTP/1.1 " << Utils::intToString(status_code) << " " << getStatusMessage(status_code) << "\r\n";
	
	// Build headerMap first
	
	std::map<std::string, std::string>::iterator it;
	for (it = this->headerMap.begin(); it != this->headerMap.end(); it++) {
		buf << it->first << ": " << it->second << "\r\n";
	}

	this->header = buf.str();
}

void
Response::buildHeaderMap() {
	// Date: Tue, 15 Nov 1994 08:12:31 GMT
	headerMap["Date"] = getCurrentDate(); // The server MUST send the date according to the 2616 RFC 
	MLOG("DATE: " << headerMap["Date"] << "\n");
	headerMap["Content-length"] = Utils::intToString(this->body.size());
	if (this->contentType.empty())
		headerMap["Content-Type"] = this->contentType; // something
	else
		headerMap["Content-Type"] = "";
	headerMap["Cache-control"] = "no-cache, private";
	headerMap["Server"] = "";
	if (this->status_code == 302) {
		headerMap["Location"] = std::string(this->body);
		this->body.clear();
	}
}

std::string
Response::to_string(void) const {
	std::stringstream buf;

	if (!(this->cgi_response.empty())) { // There is a Cgi reponse so the rest will be empty
		return this->cgi_response;
	}

	buf << this->header;
	buf << "\r\n";
	buf << this->body;

	return buf.str();
}

bool
Response::findErrorPage(int errorCode) {
	// Find the error page corresponding to the status code
	std::map<int, std::string>::const_iterator it;
	it = this->server->getErrorMap().find(errorCode);

	// There isn't an error page defined for the specif error
	if (it == this->server->getErrorMap().end())
		return false;

		
	std::string error_path = this->server->getErrorMap()[errorCode];
	std::string path(Global::pwd + error_path);
	path = path.substr(0, path.find(';'));

	MLOG("error path :" + error_path + "path :" + path);
	if (isDirectory(path))
		return false;
	
	// Open the error page
	std::ifstream file(path.c_str());
	if (file.is_open()) {
		// Read the content of the error page
		std::stringstream buffer;
		buffer << file.rdbuf();
		this->body = buffer.str();
		file.close();
		// Generate the HTTP response with the content of the error page
		return true;
	}
	else 
	{
		MLOG("Could not open error page file: " + path);
		return false;		
	}
}

void
Response::sendToClient(int client_sock) {
	GPS;
	std::string response = this->to_string();
	MLOG("response: " << response << "\n");

	if (send(client_sock, response.c_str(), response.length(), 0) < 0) {
		throw std::runtime_error("Error: send function failed");
	}
}

const std::string
Response::getStatusMessage(int statusCode)
{
	switch (statusCode)
	{
		case 100:
			return ("Continue");
		case 101:
			return ("Switching Protocols");
		case 200:
			return ("OK");
		case 201:
			return ("Created");
		case 202:
			return ("Accepted");
		case 203:
			return ("Non-Authoritative Information");
		case 204:
			return ("No Content");
		case 205:
			return ("Reset Content");
		case 206:
			return ("Partial Content");
		case 300:
			return ("Multiple Choices");
		case 301:
			return ("Moved Permanently");
		case 302:
			return ("Found");
		case 303:
			return ("See Other");
		case 304:
			return ("Not Modified");
		case 305:
			return ("Use Proxy");
		case 307:
			return ("Temporary Redirect");
		case 400:
			return ("Bad Request");
		case 401:
			return ("Unauthorized");
		case 402:
			return ("Payment Required");
		case 403:
			return ("Forbidden");
		case 404:
			return ("Not Found");
		case 405:
			return ("Method Not Allowed");
		case 406:
			return ("Not Acceptable");
		case 407:
			return ("Proxy Authentication Required");
		case 408:
			return ("Request Timeout");
		case 409:
			return ("Conflict");
		case 410:
			return ("Gone");
		case 411:
			return ("Length Required");
		case 412:
			return ("Precondition Failed");
		case 413:
			return ("Request Entity Too Large");
		case 414:
			return ("Request-URI Too Long");
		case 415:
			return ("Unsupported Media Type");
		case 416:
			return ("Requested Range Not Satisfiable");
		case 417:
			return ("Expectation Failed");
		case 500:
			return ("Internal Server Error");
		case 501:
			return ("Not Implemented");
		case 502:
			return ("Bad Gateway");
		case 503:
			return ("Service Unavailable");
		case 504:
			return ("Gateway Timeout");
		case 505:
			return ("HTTP Version Not Supported");
		default:
			return ("Unknown");
	}
}
