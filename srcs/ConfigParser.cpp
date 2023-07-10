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
	this->delimiter = " \r\t\n";
	this->specialChar = ";{}";
}

ConfigParser::~ConfigParser(){

}

int	ConfigParser::parse(std::string &path){
	std::ifstream	file(path.c_str());
	if (file.is_open()){
		std::stringstream	buffer;

		buffer << file.rdbuf();
		std::cout << buffer.str() << std::endl;
		file.close();
		return (1);
		// return (0);
	}
	else{
		std::cout << COLOR_RED << "Error. Failed to open file" << path << COLOR_RESET << std::endl;
		return (0);
	}
}

/**
 * @brief Check for both delimiter and special character.
 * Will split both delimiter and special character, but put
 * special character back into the vector.
 */
void	ConfigParser::tokenize(std::string &str){
	size_t	begin = str.find_first_not_of(this->delimiter);
	size_t	end = str.find_first_of(this->delimiter, begin);

	while (begin != end){
		std::string token;
		if (begin != std::string::npos && end != std::string::npos){
			token = str.substr(begin, end - begin);
			begin = str.find_first_not_of(this->delimiter, end);
			end = str.find_first_of(this->delimiter, begin);
		}
		else if (begin != std::string::npos && end == std::string::npos){
			token = str.substr(begin, str.length() - begin);
			begin = str.find_first_not_of(this->delimiter, end);
		}
		if (token.find_first_of(this->specialChar) != std::string::npos && token.length() > 1){
			handleSpecialChar(token);
		}
		else{
			// this->tokens.push_back(token);
		}
	}
}

void	ConfigParser::handleSpecialChar(std::string &str){
	(void) str;
	// int	pos = str.find_first_of(this->specialChar);
	
	// std::string token = str.substr(0 ,pos);

	// if (token.length() > 0)
	// 	this->tokens.push_back(token);
	// token = str.substr(pos, 1);
	// if (token.length() > 0)
	// 	this->tokens.push_back(token);
	// std::string remain = str.substr(pos + 1, str.length() - pos - 1);
	// if (rem)

}