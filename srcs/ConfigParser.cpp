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
	this->tokenizer = Tokenizer(this->delimiter, this->specialChar);
}

ConfigParser::~ConfigParser(){

}

int	ConfigParser::parse(std::string &path){
	std::ifstream	file(path.c_str());

	if (!file.is_open()){
		std::cout << COLOR_RED << "Error. Failed to open file" << path << COLOR_RESET << std::endl;
		return (0);
	}
	std::stringstream	buffer;
	std::string keys[] = {"server", "listen", "server_name"};
	serverConf *currentConf = NULL;

	buffer << file.rdbuf();
	this->tokens = this->tokenizer.Tokenize(buffer.str());

	for (size_t i = 0;i < this->tokens.size(); i ++){
			int indent_level = 0;
			if (this->tokens[i] == "{")
				indent_level += 1;
			else if (this->tokens[i] == "}")
				indent_level -= 1;
			else if (this->tokens[i] == "server"){
				if (currentConf != NULL){
					this->serverConfs.push_back(*currentConf);
					currentConf = NULL;
				}
			}
			else{
			
			}

		}
		file.close();
		// return (1);
		return (0);
	
}