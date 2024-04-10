/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusCode.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pealexan <pealexan@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 12:28:33 by pealexan          #+#    #+#             */
/*   Updated: 2024/04/10 12:33:25 by pealexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef STATUSCODE_HPP
#define STATUSCODE_HPP

#include <string>

class StatusCode {
	public:
		StatusCode();
		~StatusCode();
		int	getStatusCode();
		void setStatusCode(int code);
		std::string getStatusMessage(int code);
		std::string generateErrorPage(int code);
	
	private:
		int	_statusCode;
};

#endif