/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 11:51:02 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/26 12:12:22 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "HttpRequest.hpp"

class HttpResponse {
	
	private:
	
	public:
		HttpResponse();
		HttpResponse(const HttpRequest& request);
		~HttpResponse();
};
