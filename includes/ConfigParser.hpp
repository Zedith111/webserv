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

# include "Tokenizer.hpp"

# include <vector>
# include <sstream>

class ConfigParser{
	private:
		Tokenizer					tokenizer;
		std::string					delimiter;
		std::string					specialChar;
		std::vector<std::string>	tokens;
		std::vector<serverConf>		serverConfs;
	public:
		ConfigParser();
		~ConfigParser();
		int	parse(std::string &path);
};

#endif