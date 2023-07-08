/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 11:10:16 by zah               #+#    #+#             */
/*   Updated: 2023/07/06 21:03:51 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

ConfigParser::ConfigParser(){

}

ConfigParser::~ConfigParser(){

}

//Separate with space, \r , \t and \n
//Special character ; { }
//Peek last character and separate out

int	ConfigParser::parse(std::string &path){
	std::ifstream	file(path);
	if (file.is_open()){
		std::stringstream	buffer;

		buffer << file.rdbuf();
		std::string	line;
		while (std::getline(buffer, line, ' ')){
			std::cout << line <<  " end" <<  std::endl;
		}
		// while (std::getline(file, line)){
		// 	this->all += line;
		// }
		// std::cout << all << std::endl;
		file.close();
		// return (1);
		return (1);
	}
	else{
		std::cout << COLOR_RED << "Error. Failed to open file" << path << COLOR_RESET << std::endl;
		return (0);
	}
}

/**
 * @brief Check whether a string a delimiter
 * @return 0 when not, 1 when is separator,2 when special character.
 * Spe
 */
// int	checkDelimiter(char c){
// 	switch (c)
// 	{
// 	case /* constant-expression */:
// 		/* code */
// 		break;
	
// 	default:
// 		break;
// 	}
// }