/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:12 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/12 12:24:17 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class CgiHandler {

	private:
		static std::map<std::string, std::string> extentionToInterpreter;

		t_request _request;
		std::string _extention;
		
		void setEnvVariables(const std::string& name, const std::string& value);
		void executeCgi();
		

	public:
		CgiHandler(const std::string& file_name, const std::string& extension);	 // Prob. not needed
		CgiHandler(const t_request& request);
		~CgiHandler();

		static void initMap(void);
};
