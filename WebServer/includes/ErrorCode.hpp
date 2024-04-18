/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorCode.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pealexan <pealexan@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 12:28:33 by pealexan          #+#    #+#             */
/*   Updated: 2024/04/10 14:10:31 by pealexan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#ifndef ERRORCODE_HPP
#define ERRORCODE_HPP

#include <string>

class ErrorCode {
	public:
		ErrorCode();
		~ErrorCode();
		int	getErrorCode();
		void setErrorCode(int errorCode);
		std::string getErrorMessage(int errorCode);
		std::string generateErrorPage(int errorCode);
	
	private:
		int	_errorCode;
};

#endif