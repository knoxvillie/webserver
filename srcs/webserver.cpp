/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/15 09:36:33 by kfaustin          #+#    #+#             */
/*   Updated: 2024/01/15 09:36:33 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include"../includes/webserver.hpp"

int	main(int argc, char* argv[]) {
	if (argc > 2) {
		std::cout << "Webserver must have only 1 argument" << std::endl;
		return (1);
	}
	const std::string config_file(argc == 1 ? "../conf/default.conf" : argv[1]);
	return (0);
}