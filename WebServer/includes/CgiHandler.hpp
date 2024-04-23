/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:12 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/23 14:09:10 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Server.hpp"

class CgiHandler {

	private:
		t_request _request;

		std::string PATH_INFO;
		std::string PATH_TRANSLATED;
		std::string QUERY_STRING;

		std::string _extension; // should be .cgi
		const char *_envp[]; // where to store the env variables
		
		std::map<std::string, std::string> buildEnv();
		void setEnvVariables(const std::map<std::string, std::string>& header);
		void executeCgi(void);
		void setPathTranslated(void);
		

	public:
		CgiHandler(const t_request& request);
		~CgiHandler();
};
