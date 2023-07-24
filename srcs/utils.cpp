/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:14:45 by zah               #+#    #+#             */
/*   Updated: 2023/07/24 14:58:48 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

void printConfig(serverConf &conf){
	std::cout << "Host: " << conf.host << std::endl;
	std::cout << "Port: " ;
	for (size_t i=0; i < conf.port_number.size(); i++){
		std::cout << conf.port_number[i] << ", ";
	}
	std::cout << std::endl;
	std::cout << "Server Name: " << conf.server_name << std::endl;
	std::cout << "Root: " << conf.root << std::endl;

}