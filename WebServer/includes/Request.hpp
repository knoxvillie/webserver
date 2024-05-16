/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 09:57:34 by diogmart          #+#    #+#             */
/*   Updated: 2024/05/16 16:01:51 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "webserv.hpp"
#include "utils.hpp"

class Request {
	
	private:
		std::string full; // full request unparsed

		std::string request_line; // First line
		std::string header;
		std::string body;
		
		std::map<std::string, std::string> headerMap;


		// Info from the header
		std::string http_version;
		std::string method; // GET POST DELETE
		std::string uri;
		std::string file_path;
		int content_length;
		int bytes_read;

		// CGI
		bool cgi;
		std::string path_info;
		std::string query_string;
	
		void requestParser(void);
		void ParseURL(void);
		
		bool finished; // whether the request has finished and can be processed or if it still needs to receive more data
		bool keep_alive;
		bool chunked;

		void setContentLength(void);
		void setEnconding(void);
		void setConnection(void);

	public:
		Request(Server* server);
		Request(const std::string& request);
		~Request();

		t_location *location;
		Server *server;

		//void decodeURI(void);
		void receiveData(const std::string& buf, int bytes);
		void receiveChunked(const std::string& buf, int bytes);

	//	Getters	//
		const std::string& getFull(void) const;
		const std::string& getHeader(void) const;
		const std::map<std::string, std::string>& getHeaderMap(void) const;
		const std::string& getBody(void) const;
		const std::string& getHttpVersion(void) const;
		const std::string& getMethod(void) const;
		const std::string& getURI(void) const;
		const std::string& getFilePath(void) const;
		int getContentLength(void) const;
		bool isCGI(void) const;
		bool isChunked(void) const;
		bool isFinished(void) const;
		bool isToClose(void) const;
		const std::string& getQueryString(void) const;
		const std::string& getPathInfo(void) const;
		std::string getContentType(void);
		
	//	Setters	//
		void setHeader(void);
		void fillHeaderMap(void);
		void setBody(void);
		void setHttpVersion(const std::string& http_version);
		void setMethod(const std::string& method);
		void setFilePath(const std::string& file_path);
		void setURI(const std::string& uri);
		void setToClose(void);
};

#endif //REQUEST_HPP