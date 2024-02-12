/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 12:00:44 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/09 12:26:26 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

std::vector<std::string> Config::directives;

void
Config::initDirectivesVec() {
	Config::directives.push_back("listen"); 
	Config::directives.push_back("server_name");
	Config::directives.push_back("host");
	Config::directives.push_back("root");
	Config::directives.push_back("index");
	Config::directives.push_back("charset");
	Config::directives.push_back("access_log");
	Config::directives.push_back("error_log");
	Config::directives.push_back("error_page");
	Config::directives.push_back("location");
	Config::directives.push_back("client_max_body_size");
	// If needed add more directives here
}

std::vector<std::string>
Config::getDirectives() {
	return (Config::directives);
}

/* ===================================== */

Config::Config() {}

Config::Config(const Config& obj) {
	(void) obj;
	// ...
}

Config::~Config() {}
