/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:12 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/16 15:14:12 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class CgiHandler {

	private:
		static std::map<std::string, std::string> extensionToInterpreter;

		t_request _request;

		std::string PATH_INFO;
		std::string QUERY_STRING;

		std::string _extension;
		const char *_interpreter; // Pass the interpreter string into this pointer
		const char *_envp[]; // where to store the env variables
		
		std::map<std::string, std::string> buildEnv();
		void setEnvVariables(const std::map<std::string, std::string>& header);
		void executeCgi(void);
		void getQueryString(void);
		void getPathInfo(void);
		

	public:
		CgiHandler(const std::string& file_name, const std::string& extension);	 // Prob. not needed
		CgiHandler(const t_request& request);
		~CgiHandler();

		static void initMap(void);
};
