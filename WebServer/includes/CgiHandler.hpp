/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:12 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/09 13:56:56 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"

class CgiHandler {

	private:
		static std::map<std::string, std::string> extentionToInterpreter;
		
		std::string extention;
		std::string file;

	public:
		CgiHandler(const std::string& file_name, const std::string& extension);	
		~CgiHandler();

		static void initMap(void);
};
