/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 10:35:43 by diogmart          #+#    #+#             */
/*   Updated: 2024/06/01 14:17:33 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(Server* arg) : bytes_read(0), cgi(false), finished(false), keep_alive(true), chunked(false), server(arg), cgi_finished(false) {}


Request::Request(const std::string& request) : full(request), bytes_read(0), cgi(false), finished(false), keep_alive(true), chunked(false), cgi_finished(false) {
	// this is wrong, the request should be initialized with the default constructor and then
	// data should be added and parsed when its done receiving data
	// But idk could be useful to have this so I'm just gonna leave it
	setHeader();
	fillHeaderMap();
	ParseURL();
	setBody();
}

Request::~Request(void) {}

void
Request::receiveData(const std::string& buf, int bytes) {

	if (bytes == 0) { // this indicates that the client has close the connection gracefully
		this->setToClose();
		this->finished = true;
		return;
	}
	if (bytes == -1) { // this means its an error
		this->setToClose();
		this->finished = true;
		return;
	}

	this->full.append(buf); // append the new data to the request

	if ((this->full.find("\r\n\r\n") != std::string::npos) && this->header.empty()) { // End of the header. only enter when header is empty
		this->setHeader();
		this->fillHeaderMap();

		// Get the size of the body content in this buffer, to compare with content length
		int body_bytes = this->full.substr(this->full.find("\r\n\r\n") + 4).size();
		
		if (this->isChunked()) {
			this->receiveChunked(this->full.substr(this->full.find("\r\n\r\n") + 4));
			return;
		}
		else {
			this->bytes_read += bytes;
			this->bytes_read -= (bytes - body_bytes); // Only need to do this once, when the header is found
		}

		if (this->content_length == -1 || this->content_length == 0) // The header is read so if there is no content length the request should be complete
			this->finished = true;

		if ((this->content_length != -1) && (this->bytes_read >= this->content_length))
			this->finished = true;

		MLOG("================================================\nHEADER IS FILLED\n================================================");

		return;
	}

	if (this->header.empty()) return; // The full header hasn't been read yet
	
	if (this->isChunked()) { // Handle chunked requests
		this->receiveChunked(buf);
		return;
	}

	this->bytes_read += bytes; // The content length is only for the body. so subtract header bytes when header is found
	MLOG("BYTES READ: " << bytes_read);
	MLOG("FULL SIZE: " << full.size())
	MLOG("HEADER SIZE: " << header.size());
	
	if ((this->content_length != -1) && (this->bytes_read >= this->content_length))
		this->finished = true;
}

void
Request::receiveChunked(const std::string& buf) {
	MLOG("CHUNKED REQUEST\n");
	GPS;

	// Note: No data that belongs to the header will be sent here because of the receiveData() function
	//		 so we only need to handle the body portion

	this->chunkbuf.append(buf);

	while (!this->chunkbuf.empty()) {
		
		if (this->chunkbuf.find("0\r\n") != std::string::npos) // Terminating empty chunk has been found
			this->finished = true;
		
		// the chunk always starts with the size of the chunk in hex
		unsigned long chunk_size = std::strtoul(this->chunkbuf.substr(0, this->chunkbuf.find("\r\n")).c_str(), NULL, 16); // converts hex to decimal already
		if (chunk_size <= 0)
			break;

		unsigned long total_chunksize = Utils::intToString(chunk_size).size() + chunk_size + 4;
		if (total_chunksize > this->chunkbuf.size()) // The chunk is not completed yet
			break;	

		// Create a substring from the end of <size>/r/n up to <size> bytes. e.g. from "10/r/nABCDEFGHIJ\r\n" extract "ABCDEFGHIJ"
		std::string chunk = this->chunkbuf.substr(Utils::intToString(chunk_size).size() + 2, chunk_size); // Chunk_size + 1 since last is excluded ??
		MLOG("CHUNK: " << chunk);
		this->body.append(chunk);
		this->chunkbuf.erase(0, total_chunksize);
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
		this->path_info = "";
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
	
	if (!extension.empty())
		this->extension = extension;
	else
		this->extension = "";

	MLOG("Extension: " << this->extension << "\n\n");
	
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
	GPS;
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
Request::getExtension(void) const {
	return this->extension;
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
	if (!full.empty() && (full.find("\r\n\r\n") + 4) != std::string::npos)
		this->body = full.substr(full.find("\r\n\r\n") + 4);
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
	if (this->headerMap.find("Content-Length") != this->headerMap.end())
		content_length = std::atoi(this->headerMap["Content-Length"].c_str());
}

void
Request::setEnconding(void) {
	// Transfer-Encoding tells us if the request is chunked or not
	if (this->headerMap.find("Transfer-Encoding") == this->headerMap.end()) {
		this->chunked = false;
		return;
	}
	
	this->chunked = false;
	if (this->headerMap["Transfer-Encoding"].compare("chunked") == 0)
	{
		this->chunked = true;
		MLOG(this->chunked);
	}
}

void
Request::setConnection(void) {
	if (this->headerMap.find("Connection") != this->headerMap.end())
		if (this->headerMap["Connection"].compare("close") == 0)
			this->keep_alive = false;
}

void
Request::setToClose(void) {
	this->keep_alive = false;
}

void
Request::setCGI(void) {
	this->cgi = true;
}

std::string
Request::getContentType(void) {
	if (this->headerMap.find("Content-Type") != this->headerMap.end())
		return (this->headerMap["Content-Type"]);
	return "";
}
