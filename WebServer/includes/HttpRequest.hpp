/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 11:50:54 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/30 23:44:05 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

#define BUFFER_SIZE 4096

#include "webserv.hpp"
#include "Server.hpp"

class HttpRequest {
	
	private:
		HttpRequest(void);
		
		std::string request;

		std::string method;
		std::string uri;

	public:
		HttpRequest(int, const Server*);
		~HttpRequest();

		void parser(const Server*);
		void readRequest(int);
		void sendResponse(int);
};

#endif
