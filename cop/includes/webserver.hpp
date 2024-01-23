/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 09:36:33 by kfaustin          #+#    #+#             */
/*   Updated: 2024/01/15 11:07:24 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#pragma once

#ifdef DEBUG
# define MESSAGE(a,b) std::cout << "DEBUG" << std::endl;
#else
# define MESSAGE
#endif

//C++ Standard Libraries
#include <iostream>
#include <string>
#include <fstream>

#endif