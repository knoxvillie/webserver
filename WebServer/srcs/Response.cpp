/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/16 11:02:15 by kfaustin          #+#    #+#             */
/*   Updated: 2024/04/16 15:32:50 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(t_request& obj) : request(obj){

}

/*
static void
doResponse(const std::string& content, int status_code, int& clientSock) {
	std::ostringstream oss;

	oss << "HTTP/1.1" << " " << status_code << "\r\n";
	oss << "Cache-Control: no-cache, private\r\n";
	oss << "Content-Type: text/html\r\n";
	oss << "Content-Length: " << content.length() << "\r\n";
	oss << "\r\n";
	oss << content;

	// Send the response to the client
	if (send(clientSock, oss.str().c_str(), oss.str().length(), 0) < 0) {
		throw std::runtime_error("Error: send function failed");
	}
}*/
