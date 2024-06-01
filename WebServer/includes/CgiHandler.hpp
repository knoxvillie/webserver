/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:12 by diogmart          #+#    #+#             */
/*   Updated: 2024/06/01 14:27:20 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Http.hpp"

class Request;
class Response;

class CgiHandler { // Should this be static ?

	private:
		CgiHandler();
		~CgiHandler();

		static char **buildEnv(Request& request);
		static const std::string getPathTranslated(Request& request);

	public:
		static Response* executeCgi(Request& request);
		static void readFromCgi(int fd, Request& request);
		static void writeToCgi(int fd, Request& request);
};
