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
# include <sys/time.h>
# include <sys/stat.h>
# include <fstream>
# include <netdb.h>
# include <iostream>
# include <string>
# include <sstream>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <cstring>
# include <fcntl.h>
# include <vector>
# include <map>
# include <exception>

# define COLOR_RESET   "\033[0m"
# define COLOR_RED     "\033[31m"
# define COLOR_GREEN   "\033[32m"
# define COLOR_YELLOW  "\033[33m"

enum METHOD{
	NOT_ALLOWED = -2,
	NOT_IMPLEMENTED = -1,
	GET = 0,
	POST,
	PUT,
	HEAD,
	DELETE,
	METHOD_COUNT,
};

struct locationInfo{
	std::string					root;
	std::string					index;
	std::string					redirect_address;
	std::string 				upload_path;
	bool 						autoindex;
	bool						is_directory;
	std::vector<std::string>	limit_except;
	std::string::size_type		max_body_size;
};

/**
 * @brief Struct to store sever configuration. Obtained through a config file
 * and use to initialize the server.
 * @param host The host of the server, can be either an IP address or a domain name.
 * Port number will be excluded. Only one unique host is allowed.
 * @param port_number A vector of port numbers to listen to. The port number is stored as string
 * @param server_name The name of the server. Can be any string.
 * @param root The root directory of the server. All files will relative to this directory.
 * @param error_pages A map of error pages. The key is the status code, and the value is the path to the error page.
 * @param cgi A multimap of cgi. The key is the path to the cgi, and the value is the path to the executable. Multiple cgi extension
 * can be mapped to the same executable.
 * @param locations A map of locationInfo. The key is the location path, and the value is the locationInfo struct.
 * 
 */
struct serverConf{
	std::string host;
	std::vector<std::string>	port_number;
	std::string server_name;
	std::string root;
	std::map<int, std::string>	error_pages;
	std::multimap<std::string, std::string> cgi;
	std::map<std::string, locationInfo *> locations;
};

# ifndef DEBUG
#  define DEBUG 0
# endif



#endif