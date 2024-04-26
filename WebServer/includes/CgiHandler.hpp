/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:12 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/26 13:20:16 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Server.hpp"

class CgiHandler { // Should this be static ?

	private:
		t_request _request;

		const char *_envp[]; // where to store the env variables
		
		void buildEnv(void);
		// void setEnvVariables(const std::map<std::string, std::string>& header);
		const std::string getPathTranslated(void);
		
		void executeCgi(void);
		
		void writeToCgi(int fd, const std::string& content);
		std::string readFromCgi(int& fd);

	public:
		CgiHandler(const t_request& request);
		~CgiHandler();
};
