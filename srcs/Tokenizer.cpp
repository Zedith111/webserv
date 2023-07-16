/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 16:50:23 by zah               #+#    #+#             */
/*   Updated: 2023/07/15 18:19:39 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Tokenizer.hpp"

Tokenizer::Tokenizer(){

}

Tokenizer::Tokenizer(std::string &delim, std::string &special){
	this->delim = delim;
	this->special = special;
}

Tokenizer::~Tokenizer(){

}

std::vector<std::string>	&Tokenizer::Tokenize(std::string msg){
	this->msg = msg;
	this->begin = msg.find_first_not_of(delim);
	this->end = msg.find_first_of(delim, this->begin);
	while (begin != end)
		getNextToken();
	return (this->tokens);
}

void	Tokenizer::display(){
	for (size_t i = 0; i < this->tokens.size(); i++)
		std::cout << this->tokens[i] << std::endl;
}

void	Tokenizer::getNextToken(){
	std::string	temp;

	if (begin != std::string::npos && end != std::string::npos){
		temp = msg.substr(begin, end - begin);
		this->begin = msg.find_first_not_of(delim, end);
		this->end = msg.find_first_of(delim, begin);
	}
	else if (begin != std::string::npos && end == std::string::npos){
		temp = msg.substr(begin, msg.length() - begin);
		this->begin = msg.find_first_not_of(delim, end);
	}
	if (temp.find_first_of(special) != std::string::npos && temp.length() > 1)
		handleSpecial(temp);
	else
		this->tokens.push_back(temp);
}


void	Tokenizer::handleSpecial(std::string &current){
	std::string::size_type	specialChar = current.find_first_of(special);
	std::string::size_type	normalChar = current.find_first_not_of(special);
	std::string				temp;

	//String consist only special character
	if (normalChar == std::string::npos){
		for (size_t i = 0; i < current.length(); i++){
			temp = current.substr(i, 1);
			this->tokens.push_back(temp);
		}
	}
	else{
		std::string::size_type begin = 0;
		std::string::size_type end;
		while (normalChar != specialChar){
			//Special character is in the front, split by one
			if (normalChar > specialChar){
				end = 1;
				temp = current.substr(begin, end - begin);
				this->tokens.push_back(temp);
				begin += 1;
			}
			else{
				end = current.find_first_of(special, begin);
				temp = current.substr(begin, end - begin);
				this->tokens.push_back(temp);
				begin = end;
			}
			specialChar = current.find_first_of(special, begin);
			normalChar = current.find_first_not_of(special, begin);
		}
	}
}