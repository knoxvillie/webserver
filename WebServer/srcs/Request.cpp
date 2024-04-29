/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 10:35:43 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/29 11:30:14 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(void) {}

// TODO
Request::Request(const std::string& request) : full(request) {
	// this is wrong, the request should be initialized with the default constructor and then
	// data should be added and parsed when its done receiving data
	setHeaderAndBody();
	fillHeaderMap();
	ParseURL();
}

Request::~Request(void) {}

void
Request::receiveData(const std::string& buf) {
	full.append(buf); // append the new data to the request
	// No need to check if the request is finished, when its finished the EPOLLIN event
	// will stop, telling us that there is no more data to receive
}

void
Request::ParseURL(void) {
	MLOG("\n\nUNPARSED URL: " + this->uri + "\n\n");
	std::string extension, url = this->uri;
	size_t pos;

	pos = url.find(".");
	if (pos == std::string::npos) {
		this->path_info = "/";
		if ((pos = url.find("?")) && pos != std::string::npos) { // if there is a query_string it will be ignored but store it anyway
			this->query_string = url.substr(pos + 1);
			this->uri = url.substr(0, pos);
		} else
			this->uri = url;
		MLOG("PARSED URL: " + this->url + "\n\n");
		MLOG("QUERY STRING: " + this->query_string + "\n\n");
		return;
	} else
		extension = url.substr(pos);

	pos = extension.find("?"); // There is a query_string
	if (pos != std::string::npos) {
		this->query_string = extension.substr(pos + 1);
		extension = extension.substr(0, pos);

		MLOG("QUERY STRING: " + this->query_string + "\n\n");
	}
	pos = extension.find("/");
	if (pos != std::string::npos) { // There is path_info
		this->path_info = extension.substr(pos); // no need for extension.find("?") because we already remove query string before
		extension = extension.substr(0, pos);

		MLOG("PATH INFO: " + this->path_info + "\n\n");
	}
	
	if (extension != ".cgi") this->cgi = false;
	else this->cgi = true;

	MLOG("CGI: " << this->cgi << "\n\n");
	
	this->uri = url.substr(0, (url.find(extension) + extension.length()));
	MLOG("PARSED URL: " + this->uri + "\n\n");
}

/* void
Request::decodeURI(void) {
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

//	Setters	//
void
Request::setHeaderAndBody(void) {
	std::string& full_request = this->full;

	this->request_line = full_request.substr(0, full_request.find("\r\n"));
	this->header = full_request.substr((this->request_line).length() + 2, full_request.find("\r\n\r\n"));
	this->body = full_request.substr(full_request.find("\r\n\r\n") + 1, std::string::npos);
}

void
Request::fillHeaderMap(void) {
	size_t pos = 0;
	std::string line;
	std::string header(this->header);

	while ((pos = header.find("\r\n")) != std::string::npos) {
		line = header.substr(0, pos);
		header.erase(0, pos + 2);
		if (line.empty())
			break;
		size_t colPos = line.find(":");

		if (colPos != std::string::npos) {
			std::string key = line.substr(0, colPos);
			std::string value = line.substr(colPos + 2, std::string::npos); // +2 because of whitespace after ":"
			this->headerMap[key] = value;
		}
	}
}

// Getters

const std::string&
Request::getFull(void) const {
	return this->full;
}

const std::string&
Request::getHeader(void) const {
	return this->header;
}

const std::map<std::string, std::string>&
Request::getHeaderMap(void) const {
	return this->headerMap;
}

const std::string&
Request::getBody(void) const {
	return this->body;	
}

const std::string&
Request::getMethod(void) const {
	return this->method;
}

const std::string&
Request::getHttpVersion(void) const {
	return this->http_version;
}

const std::string&
Request::getURI(void) const {
	return this->uri;
}

const std::string&
Request::getFilePath(void) const {
	return this->file_path;
}

bool
Request::isCGI(void) const {
	return this->cgi;
}

const std::string&
Request::getQueryString(void) const {
	return this->query_string;
}

const std::string&
Request::getPathInfo(void) const {
	return this->path_info;
}

// Setters

void
Request::setHttpVersion(const std::string& http_version) {
	this->http_version = http_version;
}

void
Request::setMethod(const std::string& method) {
	this->method = method;
}

void
Request::setFilePath(const std::string& file_path) {
	this->file_path = file_path;
}

void
Request::setURI(const std::string& uri) {
	this->uri = uri;
	ParseURL();
}
