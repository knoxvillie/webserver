/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 11:35:52 by diogmart          #+#    #+#             */
/*   Updated: 2024/03/25 11:42:43 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "webserv.hpp"
#include "Parser.hpp"
#include "TcpServer.hpp"
#include "Server.hpp"

extern std::vector<TcpServer*> servers;

void createServers(void);
void deleteServers(void);
