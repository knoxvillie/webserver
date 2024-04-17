/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 11:13:58 by diogmart          #+#    #+#             */
/*   Updated: 2024/04/16 15:32:44 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "webserv.hpp"

class Response {
	
	private:
		t_request request;
	
	public:
		Response(t_request&);
		~Response(void);
		void findErrorPage(int&);
		//static void doResponse(const std::string&, int, int&);

};

#endif //RESPONSE_HPP