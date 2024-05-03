/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:58 by diogmart          #+#    #+#             */
/*   Updated: 2024/05/03 09:09:49 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"
#include "utils.hpp"
#include "Server.hpp"

class Response {
	
	private:
		int status_code;
		std::string header;
		std::map<std::string, std::string> headerMap;
		std::string body;
		std::string contentType;
		std::string cgi_response;
		int body_length;
		
		Server *server;

		bool isError;

		void buildHeader();
		void buildHeaderMap();
		bool findErrorPage(void);
	
	public:
		Response(void);
		Response(const std::string& content);
		Response(int statusCode, const std::string& content);
		Response(int statusCode, const std::string& content, const std::string& type);
		Response(int errorCode, Server* server);

		~Response(void);

		void findErrorPage(int&);
		//static void doResponse(const std::string&, int, int&);

		std::string to_string(void) const;
		void sendToClient(int client_sock);

		// Here or in Http ?
		static const std::string createResponse(int statusCode, const std::string& content);
		static const std::string generateErrorPage(int errorCode);
		static const std::string getStatusMessage(int statusCode);

};

#endif //RESPONSE_HPP
