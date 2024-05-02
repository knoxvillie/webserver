/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:08:41 by kfaustin          #+#    #+#             */
/*   Updated: 2024/05/02 12:44:27 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef HTTP_HPP
# define HTTP_HPP

#include "webserv.hpp"
#include "Server.hpp"
#include "CgiHandler.hpp"
#include "Request.hpp"
#include "Response.hpp"

class Http {
	
	private:
		Http(void);

		int _clientSock;
		Server* _server;
		std::string _decodedURI;

		std::string http_version;
		t_request request;

	public:
		Http(int, Server*);
		~Http();

		//	Request
		static void receiveFromClient(int socket, Request& request);
		static void requestParser(Request& request);

		//	Response
		static std::string directoryListing(Request& request);
		static Response* BuildResponse(Request& request);
		static Response* doDirectoryResponse(Request& request, bool is_redirect);
		void doResponse(const std::string&, const std::string&, int status_code, int& clientSock);
		void findErrorPage(int);
		
		// Error
		void httpError(int status_code);

		static Response* handleMethod(Request& request);
		static int getMethod(const std::string& file_path, const std::vector<std::string>&, std::string& content);
		static int postMethod(const std::string& file_path, const std::vector<std::string>&, const std::string& body);
		static int deleteMethod(const t_location *location);

		void checkBodySize(t_location*);
		void setHeaderAndBody(void);
		void fillHeaderMap(void);
		void ParseURL(void);
		void decodeURI(void);

		// Getters
		std::string getMethodType();
		std::string getHeader();
		std::string getBody();
		std::string getURI();
};

#endif
