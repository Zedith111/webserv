/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/15 16:05:59 by zah               #+#    #+#             */
/*   Updated: 2023/07/15 18:04:12 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZER_HPP
# define TOKENIZER_HPP

# include "Webserv.hpp"
# include <vector>

/**
 * @param msg Duplicate of the string to be tokenized. The original string is not modified.
 * @param delim The delimiter to be used to split the string.
 * @param special The special characters to be used to split the string. Unlike delimiter,
 * special characters will be include in tokens.
 * @param tokens The result of the tokenization.
 * @param begin The index of the first character of the next token.
 * @param end The index of the last character of the next token.
 * 
 */
class Tokenizer{
	private	:
		std::string	msg;
		std::string	delim;
		std::string	special;
		std::vector<std::string> tokens;
		std::string::size_type	begin;
		std::string::size_type	end;

		Tokenizer();

		//Class member function
		void	getNextToken();
		void	handleSpecial(std::string &current);

	public :
		Tokenizer(std::string &msg, std::string &delim, std::string &special);
		~Tokenizer();
		void	Tokenize();
		std::vector<std::string> getTokens();
		void	display();
};

#endif