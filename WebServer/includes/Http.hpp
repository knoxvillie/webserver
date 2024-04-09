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

		std::string request;
		std::string method;
		std::string url;
		std::string http_version;
		std::string file_path;

	public:
		Http(int, Server*);
		~Http();

		//	Request
		void requestFromClient(void);
		void requestParser(void);
		//	Response
		void responseSend(void);
		void generateErrorResponse(std::ostringstream&, int);
		
		int getMethod(const t_location *location, std::string& content);
		int postMethod(const t_location *location);
		int deleteMethod(const t_location *location);
		bool isCGI(const std::string& file);
};

#endif
