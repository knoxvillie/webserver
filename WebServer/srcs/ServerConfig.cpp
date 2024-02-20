/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/09 12:00:44 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/20 12:34:09 by diogmart         ###   ########.fr       */
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

void ServerConfig::setConfigFileVariables(const std::map<std::string, std::vector<std::string> >& directives) {

	for (std::map<std::string, std::vector<std::string> >::const_iterator it = directives.begin(); it != directives.end(); it++) {
		std::string value_sum;
		for (size_t i = 0; i < it->second.size(); i++)
			value_sum += " " + it->second[i];
		this->changeVariable(it->first, value_sum);
		// Should we add the string to store them or just save them as a vec ???
	}
}

bool ServerConfig::changeVariable(const std::string &name, const std::string& value) {
	
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
	} else if (name == "location") {
		this->m_location = value;
		return true;
	} else if (name == "client_max_body_size") {
		this->m_client_max_body_size = value;
		return true;
	}
	return false;
}



/*
========================================
				GETTERS						
========================================
*/


// Listen
std::string ServerConfig::getListen() const {
	return this->m_listen;
}


// Server name
std::string ServerConfig::getServerName() const {
	return this->m_server_name;
}


// Host
std::string ServerConfig::getHost() const {
	return this->m_host;
}


// Root
std::string ServerConfig::getRoot() const {
	return this->m_root;
}


// Index
std::string ServerConfig::getIndex() const {
	return this->m_index;
}


// Charset
std::string ServerConfig::getCharset() const {
	return this->m_charset;
}


// AccessLog
std::string ServerConfig::getAccessLog() const {
	return this->m_access_log;
}


// Error log
std::string ServerConfig::getErrorLog() const {
	return this->m_error_log;
}


// Error page
std::string ServerConfig::getErrorPage() const {
	return this->m_error_page;
}


// Location
std::string ServerConfig::getLocation() const {
	return this->m_location;
}


// Client max body size
std::string ServerConfig::getClientMaxBodySize() const {
	return this->m_client_max_body_size;
}
