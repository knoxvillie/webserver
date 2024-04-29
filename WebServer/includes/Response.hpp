/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:58 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/29 11:56:17 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"

class Response {
	
	private:
		int status_code;
		int body_length;
		std::string header;
		std::map<std::string, std::string> headerMap;
		std::string body;
		
		bool isError;
	
	public:
		Response(void);
		Response(int errorCode);
		~Response(void);

		void findErrorPage(int&);
		//static void doResponse(const std::string&, int, int&);

		// Here or in Http ?
		static const std::string& createResponse(int statusCode, const std::string& content);
		static const std::string& generateErrorPage(int errorCode);
		static const std::string& getStatusMessage(int statusCode);

};

#endif //RESPONSE_HPP