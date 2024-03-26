/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 11:56:42 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/26 14:42:22 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpResponse.hpp"

HttpResponse::HttpResponse() {}

HttpResponse::HttpResponse(const HttpRequest& request) {
	
}

HttpResponse::~HttpResponse() {}

/*
1XX (informational)
    The request was received, continuing process.
2XX (successful)
    The request was successfully received, understood, and accepted.
3XX (redirection)
    Further action needs to be taken in order to complete the request.
4XX (client error)
    The request contains bad syntax or cannot be fulfilled.
5XX (server error)
    The server failed to fulfill an apparently valid request.
*/

std::string
HttpResponse::getFile(std::string file_name) {
	
	std::ifstream file(file_name);
	std::string content, line;

	if (!file.is_open())
		; // error;
	
	while (getline(file, line))
		content += line + '\n';
	
	return content;
}
