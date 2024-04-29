/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:02:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/29 12:07:06 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response() {}

/*
static void
doResponse(const std::string& content, int status_code, int& clientSock) {
	std::ostringstream oss;

	oss << "HTTP/1.1" << " " << status_code << "\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << content.length() << "\r\n";
	oss << "\r\n";
	oss << content;

	// Send the response to the client
	if (send(clientSock, oss.str().c_str(), oss.str().length(), 0) < 0) {
		throw std::runtime_error("Error: send function failed");
	}
}*/

const std::string&
Response::createResponse(int statusCode, const std::string& content) {
	std::stringstream buf;

	buf << "HTTP/1.1" << std::to_string(statusCode) << getStatusMessage(statusCode) << "\r\n";
	// buff << header... << "\r\n";
	buf << "Content-length: " << content.length() << "\r\n";
	buf << "Content-type: " << "text/plain" << "\r\n"; // placeholder
	buf << "\r\n";
	buf << content;

	return buf.str();
}

const std::string&
Response::generateErrorPage(int errorCode)
{
	std::string res;
	std::string message = getStatusMessage(errorCode);
	std::string code = std::to_string(errorCode);
	
	res = "<html><head><title>";
	res += code;
	res += " ";
	res += message;
	res += "</title></head><body><center><h1>";
	res += code;
	res += " ";
	res += message;
	res += "</h1></center><hr><center>42_WebServer</center></body></html>";

	return (res);
}

const std::string&
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
