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

class ConfigParser{
	private:
		std::string					delimiter;
		std::string					specialChar;
		// std::vector<std::string>	tokens;
	public:
		ConfigParser();
		~ConfigParser();
		void	tokenize(std::string &str);
		void	handleSpecialChar(std::string &str);
		int	parse(std::string &path);
};

#endif