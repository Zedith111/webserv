/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 17:05:00 by zah               #+#    #+#             */
/*   Updated: 2023/07/06 15:15:09 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

# include "Webserv.hpp"

# include <vector>
# include <sstream>

// int	isDelim(char c);
// std::vector<std::string> tokenize(std::string line);
// void read();

class ConfigParser{
	private:
		// std::string all;
		// std::vector<std::string>	tokens;
	public:
		ConfigParser();
		~ConfigParser();
		int	parse(std::string &path);
};

int		checkDelimiter(char c);

#endif