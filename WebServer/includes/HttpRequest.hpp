/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 11:50:54 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/26 15:28:00 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

#include "webserv.hpp"

class HttpRequest {
	
	private:
		HttpRequest(void);
		
		std::string request;

		std::string method;
		std::string uri;
		std::string http_version;
		std::map<std::string, std::vector<std::string> > header_fields;

	public:
		HttpRequest(char *);
		~HttpRequest();
		void parser(void);
};

#endif
