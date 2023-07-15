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

Tokenizer::Tokenizer(std::string &msg, std::string &delim, std::string &special){
	this->msg = msg;
	this->delim = delim;
	this->special = special;

	this->begin = msg.find_first_not_of(delim);
	this->end = msg.find_first_of(delim, this->begin);
}

Tokenizer::~Tokenizer(){

}

void	Tokenizer::Tokenize(){
	while (begin != end)
		getNextToken();
}

std::vector<std::string> Tokenizer::getTokens(){
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

/**
 * @brief In handle special, there input has 4 cases
 * 1. Only special character 
 * 2. In the front of the string
 * 3. In the middle of the string
 * 4. In the back of the string
 * @param current 
 */
void	Tokenizer::handleSpecial(std::string &current){
	std::string	temp;
	std::string	res;
	std::string::size_type	special = current.find_first_of(this->special);
	std::string::size_type	normal = current.find_first_not_of(this->special);

	if (normal == std::string::npos){
		for (size_t i = 0; i < current.length(); i++)
			this->tokens.push_back(current.substr(i, 1));
	}
	else if ()
	else{
		temp = current.substr(0, special);
		this->tokens.push_back(temp);
		temp = current.substr(special, 1);
		this->tokens.push_back(temp);
		temp = current.substr(special + 1, current.length() - special - 1);
		this->tokens.push_back(temp);
	}

	while (special != normal){
		handleSpecial(res);
	}
	
}