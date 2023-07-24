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
		std::vector<serverConf*>		serverConfs;


		int	parseToken();
		int	parseServer(size_t &current, int indent_level, serverConf *current_conf);
		int	parseLocation(size_t &current, int indent_level, serverConf *current_conf);
		int	addPort(std::string &port);

		int	parseListen(size_t &current, serverConf *current_conf);
		int	parseServerName(size_t &current, serverConf *current_conf);
		int	parseRoot(size_t &current, serverConf *current_conf);

	public:
		ConfigParser();
		~ConfigParser();
		int	parse(std::string &path);
		void printConf();
};

#endif