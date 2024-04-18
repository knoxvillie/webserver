/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 11:50:54 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/01 16:24:59 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef HTTP_HPP
# define HTTP_HPP

#define BUFFER_SIZE 4096

#include "webserv.hpp"
#include "Server.hpp"

class Http {
	
	private:
		Http(void);

		int _client;
		Server* _server;
		std::string _decodedURI;

		std::string http_version;
		t_request request;

	public:
		Http(int, Server*);
		~Http();

		//	Request
		void requestFromClient(void);
		void requestParser(void);
		//	Response
		std::string directoryListing(void);
		void responseSend(void);
		void generateErrorResponse(std::ostringstream&, int);
		
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
