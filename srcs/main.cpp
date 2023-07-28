/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/26 16:42:49 by zah               #+#    #+#             */
/*   Updated: 2023/07/06 21:13:39 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Server.hpp"
#include "ConfigParser.hpp"
#include "utils.hpp"

int	main(int argc, char **argv){

	std::string	filePath = "config/default.conf";
	if (argc > 2){
		std::cout << COLOR_RED << " Error. Usage : webserv <Configuration File>"  << COLOR_RESET << std::endl;
		return (1);
	}
	if (argc < 2)
		std::cout << COLOR_YELLOW << "Warning. No configuration file provided. A default file will be used" << COLOR_RESET << std::endl;
	else{
		filePath = argv[1];
	}
	ConfigParser confParser;
	if (!confParser.parse(filePath))
		return (1);
	std::cout << "Printing conf file: \n";
	confParser.printConf();

	//Check contain error page, if not obtain from default
	//Check error page and compulsory cgi(autoindex) can be can be used

	// Server server;
	// if (!server.init())
	// 	return (1);
	// server.run();
}


