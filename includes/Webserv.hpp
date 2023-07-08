/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/26 16:42:31 by zah               #+#    #+#             */
/*   Updated: 2023/07/06 12:10:59 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <iostream>
# include <fstream>
# include <string>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>

# include "Server.hpp"
# include "ConfigParser.hpp"

# define COLOR_RESET   "\033[0m"
# define COLOR_RED     "\033[31m"
# define COLOR_GREEN   "\033[32m"
# define COLOR_YELLOW  "\033[33m"

# ifndef DEBUG
#  define DEBUG 0
# endif

// class Webserv{
// 	private:
// 	public:
// 		Webserv();
// 		~Webserv();
// 		Webserv(const Webserv &base);
// 		Webserv	&operator=(const Webserv &rhs);

// 		init();

// };



#endif