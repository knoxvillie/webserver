/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:59:01 by diogmart          #+#    #+#             */
/*   Updated: 2023/11/13 12:00:04 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

void log(const std::string &message)
{
    std::cout << message << std::endl;
}

void exitWithError(const std::string &errorMessage)
{
    log("ERROR: " + errorMessage);
    exit(1);
}
