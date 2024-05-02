/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Http.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:08:41 by kfaustin          #+#    #+#             */
/*   Updated: 2024/05/02 15:51:04 by diogmart         ###   ########.fr       */
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
		~Http();

	public:

		//	Request
		static void receiveFromClient(int socket, Request& request);
		static void requestParser(Request& request);

		//	Response
		static std::string directoryListing(Request& request);
		static Response* BuildResponse(Request& request);
		static Response* doDirectoryResponse(Request& request, bool is_redirect);

		static Response* handleMethod(Request& request);
		static int getMethod(const std::string& file_path, const std::vector<std::string>&, std::string& content);
		static int postMethod(const std::string& file_path, const std::vector<std::string>&, const std::string& body);
		static int deleteMethod(const t_location *location);

		//static void decodeURI(void);
};

#endif
