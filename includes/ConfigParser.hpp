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
# include "utils.hpp"

# include <sstream>

class ConfigParser{
	private:
		Tokenizer					tokenizer;
		std::string					delimiter;
		std::string					special_char;
		std::vector<std::string>	tokens;
		std::vector<serverConf*>	server_confs;
		std::map<int, std::string> 	default_error_pages;


		int	parseToken();
		int	parseServer(size_t &current, int indent_level, serverConf *current_conf);
		int	parseLocation(size_t &current, int indent_level, serverConf *current_conf);

		//Server Block Parsing Functions
		int	parseListen(size_t &current, serverConf *current_conf);
		int	parseServerName(size_t &current, serverConf *current_conf);
		int	parseRoot(size_t &current, serverConf *current_conf);
		int	parseErrorPages(size_t &current, serverConf *current_conf);

		//Location Block Parsing Functions
		int	parseAutoindex(size_t &current, locationInfo *current_loc);
		int parseLocationRoot(size_t &current, locationInfo *current_loc);
		int	parseLocationIndex(size_t &current, locationInfo *current_loc);
		int	parseLimitExcept(size_t &current, locationInfo *current_loc);

		//Validating Functions
		void addErrorpages(serverConf *current_conf);
		int	validateLocationRoot(serverConf *current_conf);


		//Utility Functions
		int checkEnding(size_t &current);

	public:
		ConfigParser();
		~ConfigParser();
		int initDefaultErrorpages();
		int	parse(std::string &path);
		int validateConfig();
		void printConfs();
		std::vector<serverConf *> getConfigs();
		size_t getServerCount();
};

#endif