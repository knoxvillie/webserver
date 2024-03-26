/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 11:56:31 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/26 14:30:11 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

/*
	Example of the first request firefox does to a server:
	
	GET / HTTP/1.1
	Host: 127.0.0.1:8080
	User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:123.0) Gecko/20100101 Firefox/123.0
	Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,* / *;q=0.8
	Accept-Language: en-US,en;q=0.5
	Accept-Encoding: gzip, deflate, br
	Connection: keep-alive
	Upgrade-Insecure-Requests: 1
	Sec-Fetch-Dest: document
	Sec-Fetch-Mode: navigate
	Sec-Fetch-Site: none
	Sec-Fetch-User: ?1
	DNT: 1
	Sec-GPC: 1
*/

/*
    Host: Specifies the host and port to which the request is being sent. In this case, it's "0.0.0.0:8080".
    User-Agent: Provides information about the client making the request. In this case, it indicates that the user agent is Mozilla Firefox running on Ubuntu Linux.
    Accept: Indicates what media types the client can understand. It prefers HTML but also accepts other formats like XML and images.
    Accept-Language: Specifies the preferred language for the response. In this case, it prefers English.
    Accept-Encoding: Indicates the types of content encodings that the client supports. Here, it supports gzip and deflate compression methods.
    Connection: Specifies whether the connection should be kept alive after the current request/response cycle.
    Upgrade-Insecure-Requests: Indicates that the client would like the server to upgrade the request to a secure HTTPS connection if possible.*/


HttpRequest::HttpRequest(char* request) : request(std::string(request)) {
	
}

HttpRequest::~HttpRequest() {}
