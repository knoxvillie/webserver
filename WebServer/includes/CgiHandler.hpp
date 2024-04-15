/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:12 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/15 14:45:17 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class CgiHandler {

	private:
		static std::map<std::string, std::string> extentionToInterpreter;

		t_request _request;

		std::string PATH_INFO;
		std::string QUERY_STRING;

		std::string _extension;
		const char *_interpreter; // Pass the interpreter string into this pointer
		const char *_envp[]; // where to store the env variables
		
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
