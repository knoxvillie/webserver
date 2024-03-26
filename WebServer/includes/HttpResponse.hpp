/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 11:51:02 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/26 14:16:39 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "HttpRequest.hpp"

class HttpResponse {
	
	private:
		HttpResponse();

		std::string file_name;
		std::string status_code;
		std::string headers;
		std::string body;
		std::string content_type;
	
	public:
		HttpResponse(const HttpRequest& request);
		~HttpResponse();
};
