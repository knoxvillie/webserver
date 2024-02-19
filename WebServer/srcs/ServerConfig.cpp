/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 12:00:44 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/19 11:59:48 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

std::vector<std::string> ServerConfig::directives;

void
ServerConfig::initDirectivesVec() {
	ServerConfig::directives.push_back("listen"); 
	ServerConfig::directives.push_back("server_name");
	ServerConfig::directives.push_back("host");
	ServerConfig::directives.push_back("root");
	ServerConfig::directives.push_back("index");
	ServerConfig::directives.push_back("charset");
	ServerConfig::directives.push_back("access_log");
	ServerConfig::directives.push_back("error_log");
	ServerConfig::directives.push_back("error_page");
	ServerConfig::directives.push_back("location");
	ServerConfig::directives.push_back("client_max_body_size");
	// If needed add more directives here
}

std::vector<std::string>
ServerConfig::getDirectives() {
	return (ServerConfig::directives);
}

/* ===================================== */

ServerConfig::ServerConfig() {}

ServerConfig::ServerConfig(const ServerConfig& obj) {
	(void) obj;
	// ...
}

ServerConfig::~ServerConfig() {}
