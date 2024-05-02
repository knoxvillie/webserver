/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:12 by diogmart          #+#    #+#             */
/*   Updated: 2024/05/02 16:06:39 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

class CgiHandler { // Should this be static ?

	private:
		CgiHandler();
		~CgiHandler();

		static char **buildEnv(Request& request);
		static const std::string getPathTranslated(Request& request);
		static void writeToCgi(int fd, const std::string& content);
		static std::string readFromCgi(int& fd);

	public:
		static Response* executeCgi(Request& request);
};
