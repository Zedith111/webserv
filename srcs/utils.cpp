/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:14:45 by zah               #+#    #+#             */
/*   Updated: 2023/07/29 16:00:06 by zah              ###   ########.fr       */
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

/**
 * @brief Check if the path is a directory
 * Return 1 if is directory, 0 if is a file, -1 if error
 */
int	checkIsDirectory(std::string &path){
	struct stat file_info;
	int res = stat(path.c_str(), &file_info);
	if (res < 0){
		path = "." + path;
		res = stat(path.c_str(), &file_info);
		if (res < 0){
			return (-1);
		}
	}
	if (S_ISDIR(file_info.st_mode)) {
		return (1);
	} else {
		return (0);
	}
}

std::string intToString(int num){
	std::stringstream ss;
	ss << num;
	return (ss.str());
}