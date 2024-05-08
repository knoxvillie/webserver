/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 10:35:43 by diogmart          #+#    #+#             */
/*   Updated: 2024/05/08 15:01:17 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(Server* arg) : bytes_read(0), finished(false), keep_alive(true), chunked(false), server(arg) {}

// TODO
Request::Request(const std::string& request) : full(request), bytes_read(0), finished(false), keep_alive(true), chunked(false) {
	// this is wrong, the request should be initialized with the default constructor and then
	// data should be added and parsed when its done receiving data
	setHeader();
	fillHeaderMap();
	ParseURL();
	setBody();
}

Request::~Request(void) {}

void
Request::receiveData(const std::string& buf, int bytes) {
	this->full.append(buf); // append the new data to the request
	MLOG("Full: " << this->full << "\n");

	if (buf.find("\r\n\r\n") != std::string::npos) { // End of the header
		this->setHeader();
		this->fillHeaderMap();
	}

	if (this->isChunked()) { // Handle chunked requests
		this->receiveChunked(buf, bytes);
		return;
	}

	if (bytes <= 0) { // this indicates that the client has close the connection gracefully
		this->finished = true;
		return;
	}

	if (this->header.empty()) return; // The full header hasn't been read yet

	this->bytes_read += bytes; // The content length is only for the body
	if ((content_length != -1) && (this->bytes_read >= this->content_length))
		this->finished = true;
}

void
Request::receiveChunked(const std::string& buf, int bytes) {
	MLOG("CHUNKED REQUEST\n");
	
	if (buf.find("0\r\n") != std::string::npos)
		this->finished = true;

	for (int i = 0;	i < bytes; i++) {
		int chunk_size = std::atoi(buf.substr(i, buf.find("\r\n", i)).c_str()); // the chunk always starts with the size of the chunk
		std::string treated_data = buf.substr(buf.find("\r\n", i) + 2, std::string::npos); // the data starts after the \r\n
		
		this->body.append(treated_data);
		i += chunk_size + 3; // +3 because of the <chunk_size>\r\n
	}
}

void
Request::ParseURL(void) {
	MLOG("\n\nUNPARSED URL: " + this->uri + "\n\n");
	std::string extension, url = this->uri;
	size_t pos;

	this->cgi = false;

	pos = url.find(".");
	if (pos == std::string::npos) {
		this->path_info = "/";
		if ((pos = url.find("?")) && pos != std::string::npos) { // if there is a query_string it will be ignored but store it anyway
			this->query_string = url.substr(pos + 1);
			this->uri = url.substr(0, pos);
		} else
			this->uri = url;
		MLOG("PARSED URL: " + this->uri + "\n\n");
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
	
	if (extension == ".cgi") this->cgi = true;

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
Request::setHeader(void) {
	std::string& full_request = this->full;

	if (!full_request.empty())
		this->request_line = full_request.substr(0, full_request.find("\r\n"));
	if (!this->request_line.empty())
		this->header = full_request.substr((this->request_line).length() + 2, full_request.find("\r\n\r\n"));
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
	this->setEnconding();
	this->setConnection();
	this->setContentLength();
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

int
Request::getContentLength(void) const {
	return this->content_length;
}

bool
Request::isCGI(void) const {
	return this->cgi;
}

bool
Request::isToClose(void) const {
	return (!this->keep_alive); // True to close, false to keep alive
}

bool
Request::isChunked(void) const {
	return this->chunked;
}

bool
Request::isFinished(void) const {
	return this->finished;
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
Request::setBody(void) {
	if (this->isChunked())
		return;
	this->body = full.substr(full.find("\r\n\r\n") + 4, std::string::npos);
}

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

void
Request::setContentLength(void) {
	content_length = -1;
	if (this->headerMap.find("Content-length") != this->headerMap.end())
		content_length = std::atoi(this->headerMap["Content-length"].c_str());
}

void
Request::setEnconding(void) {
	// Transfer-Encoding tells us if the request is chunked or not
	if (this->headerMap.find("Transfer-Encoding") == this->headerMap.end()) {
		this->chunked = false;
		return;
	}
	
	this->chunked = false;
	if (this->headerMap["Transfer-Encoding"].compare("chunked\r\n") == 0)
		this->chunked = true;
}

void
Request::setConnection(void) {
	if (this->headerMap.find("Connection") != this->headerMap.end())
		if (this->headerMap["Connection"].compare("close\r\n") == 0)
			this->keep_alive = false;
}
