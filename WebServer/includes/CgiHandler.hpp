/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pealexan <pealexan@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:12 by diogmart          #+#    #+#             */
/*   Updated: 2024/06/01 15:37:14 by pealexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Http.hpp"
#include "utils.hpp"

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
