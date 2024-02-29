/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 12:00:44 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/29 14:58:43 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {}

ServerConfig::ServerConfig(const ServerConfig& obj) {
	this->m_listen = obj.m_listen;
    this->m_server_name = obj.m_server_name;
    this->m_host = obj.m_host;
    this->m_root = obj.m_root;
    this->m_index = obj.m_index;
    this->m_charset = obj.m_charset;
    this->m_access_log = obj.m_access_log;
    this->m_error_log = obj.m_error_log;
    this->m_error_page = obj.m_error_page;
    this->m_location = obj.m_location;
    this->m_client_max_body_size = obj.m_client_max_body_size;
}

ServerConfig& ServerConfig::operator=(const ServerConfig& obj) {
	if (this == &obj)
		return *this;
	
	this->m_listen = obj.m_listen;
    this->m_server_name = obj.m_server_name;
    this->m_host = obj.m_host;
    this->m_root = obj.m_root;
    this->m_index = obj.m_index;
    this->m_charset = obj.m_charset;
    this->m_access_log = obj.m_access_log;
    this->m_error_log = obj.m_error_log;
    this->m_error_page = obj.m_error_page;
    this->m_location = obj.m_location;
    this->m_client_max_body_size = obj.m_client_max_body_size;

	return *this;
}

ServerConfig::~ServerConfig() {}

void
ServerConfig::setConfigFileVariables(const std::map<std::string, std::vector<std::string> >& directives) {

	for (std::map<std::string, std::vector<std::string> >::const_iterator it = directives.begin(); it != directives.end(); it++)
		this->changeVariable(it->first, it->second);
}

bool
ServerConfig::changeVariable(const std::string &name, const std::vector<std::string>& value) {
	
	if (name == "listen") {
		this->m_listen = value;
		return true;
	} else if (name == "server_name") {
		this->m_server_name = value;
		return true;
	} else if (name == "host") {
		this->m_host = value;
		return true;
	} else if (name == "root") {
		this->m_root = value;
		return true;
	} else if (name == "index") {
		this->m_index = value;
		return true;
	} else if (name == "charset") {
		this->m_charset = value;
		return true;
	} else if (name == "access_log") {
		this->m_access_log = value;
		return true;
	} else if (name == "error_log") {
		this->m_error_log = value;
		return true;
	} else if (name == "error_page") {
		this->m_error_page = value;
		return true;
	} else if (name == "client_max_body_size") {
		this->m_client_max_body_size = value;
		return true;
	} else if (name == "location") {
		//this->m_location = value;
		return true;
	}
	return false;
}



/*
========================================
				GETTERS						
========================================
*/


// Port
std::string
ServerConfig::getListenPort() const {
	return this->m_listen[0]; // Should probably do some checking and not just return the first string
}

// IP address
std::string
ServerConfig::getHostIP() const {
	return this->m_host[0]; // Should probably do some checking and not just return the first string
}
