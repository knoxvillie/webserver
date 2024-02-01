/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: diogmart <diogmart@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/30 14:27:49 by diogmart          #+#    #+#             */
/*   Updated: 2024/02/01 13:53:56 by diogmart         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

int main(int argc, char **argv) {
    
    if (argc > 2) {
        std::cout << "Error: wrong number of arguments.\nTry: \"./webserv [configuration file]\"" << std::endl;
        return (1);
    }
    const std::string config_file(argc == 1 ? "../conf/default.conf" : argv[1]);
	MESSAGE("Config file path", config_file);
    
    return (0);
}
