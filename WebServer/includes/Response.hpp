/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:58 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/29 11:03:25 by diogmart         ###   ########.fr       */
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
		std::string body;
		//...
	
	public:
		Response(void);
		~Response(void);
		void findErrorPage(int&);
		//static void doResponse(const std::string&, int, int&);

};

#endif //RESPONSE_HPP