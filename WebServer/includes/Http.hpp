/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:08:41 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/17 10:39:30 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

		int _clientSock;
		Server* _server;
		t_request request;

	public:
		Http(int, Server*);
		~Http();

		//	Request
		void requestFromClient(void);
		void requestParser(void);
		//	Response
		std::string directoryListing(void);
		void handleResponse(void);
		void doResponse(const std::string&, const std::string&, int status_code, int& clientSock);
		void findErrorPage(int);
		void doDirectoryResponse(t_location*, bool);
		void handleMethod(t_location*);

		void checkBodySize(t_location*);
		int getMethod(const std::vector<std::string>&, std::string& content);
		int postMethod(const std::vector<std::string>&);
		int deleteMethod(const t_location *location);
		void setHeaderAndBody(void);
		void fillHeaderMap(void);
		bool isCGI(const std::string& file);
};

#endif
