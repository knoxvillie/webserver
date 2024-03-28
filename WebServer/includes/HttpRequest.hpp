/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 11:50:54 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/27 18:24:21 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

#define BUFFER_SIZE 4096

#include "webserv.hpp"
#include "TcpServer.hpp"
#include "Config.hpp"

class HttpRequest {
	
	private:
		HttpRequest(void);
		
		std::string request;

		std::string method;
		std::string uri;
		std::string http_version;

	public:
		HttpRequest(int, TcpServer&);
		~HttpRequest();

		void parser(Config&);
		void readRequest(int);
};

#endif
