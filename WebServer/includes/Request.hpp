/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 09:57:34 by diogmart          #+#    #+#             */
/*   Updated: 2024/05/02 15:10:27 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef REQUEST_HPP
# define REQUEST_HPP

#include "webserv.hpp"

class Request {
	
	private:
		std::string full; // full request unparsed

		std::string request_line; // First line
		std::string header;
		std::string body;
		
		std::map<std::string, std::string> headerMap;

		bool isFinished; // whether the request has finished and can be processed or if it still needs to receive more data

		// Info from the header
		std::string http_version;
		std::string method; // GET POST DELETE
		std::string uri;
		std::string file_path;
		std::string enconding;
		int content_length;

		// CGI
		bool cgi;
		std::string path_info;
		std::string query_string;
	
		void requestParser(void);
		void ParseURL(void);

	public:
		Request(Server* server);
		Request(const std::string& request);
		~Request();

		bool keep_alive;
		t_location *location;
		Server *server;

		//void decodeURI(void);
		void receiveData(const std::string& buf);

	//	Getters	//
		const std::string& getFull(void) const;
		const std::string& getHeader(void) const;
		const std::map<std::string, std::string>& getHeaderMap(void) const;
		const std::string& getBody(void) const;
		const std::string& getHttpVersion(void) const;
		const std::string& getMethod(void) const;
		const std::string& getURI(void) const;
		const std::string& getFilePath(void) const;
		bool isCGI(void) const;
		const std::string& getQueryString(void) const;
		const std::string& getPathInfo(void) const;
		
	//	Setters	//
		void setHeaderAndBody(void);
		void fillHeaderMap(void);
		void setHttpVersion(const std::string& http_version);
		void setMethod(const std::string& method);
		void setFilePath(const std::string& file_path);
		void setURI(const std::string& uri);
};

#endif //REQUEST_HPP