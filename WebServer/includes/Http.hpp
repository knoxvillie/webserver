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

	public:
		Http(int, Server*);
		~Http();

		void requestFromClient(void);
		void requestParser(void);
		void responseSend(void);
};

#endif
