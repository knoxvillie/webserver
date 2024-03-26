/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/26 11:50:54 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/26 14:14:20 by diogmart         ###   ########.fr       */
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

	public:
		HttpRequest(char *);
		~HttpRequest();
		void parser(void);
};

#endif
