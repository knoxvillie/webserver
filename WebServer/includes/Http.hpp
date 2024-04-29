/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:08:41 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/29 11:10:05 by diogmart         ###   ########.fr       */
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
		void requestFromClient(void);
		static void requestParser(Request& request);

		//	Response
		std::string directoryListing(void);
		void handleResponse(void);
		void doResponse(const std::string&, const std::string&, int status_code, int& clientSock);
		void findErrorPage(int);
		void doDirectoryResponse(t_location*, bool);
		void handleMethod(t_location*);
		
		// Error
		void httpError(int status_code);


		void checkBodySize(t_location*);
		int getMethod(const std::vector<std::string>&, std::string& content);
		int postMethod(const std::vector<std::string>&);
		int deleteMethod(const t_location *location);
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
