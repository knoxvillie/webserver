/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   junk.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kfaustin <kfaustin@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/30 12:38:18 by kfaustin          #+#    #+#             */
/*   Updated: 2024/01/30 13:12:43 by kfaustin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <sstream>

int	main(int argc, char** argv) {
	std::string input;
	std::string var;

	std::cout << argc << std::endl;

	std::cout << "Input: ";
	std::getline(std::cin, input);
	std::stringstream ss(input);

	while (ss >> var)
		std::cout << "Your input>" << var << std::endl;
}