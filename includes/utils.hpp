/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 12:20:00 by zah               #+#    #+#             */
/*   Updated: 2023/08/17 14:25:48 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include "Webserv.hpp"
# include "dirent.h"

int	checkIsDirectory(std::string &path);
std::string intToString(int num);
std::ostream&	operator<<(std::ostream& os, const serverConf& obj);
std::string 	printDirectory(std::string &route, std::string &file_path);

class unableToOpenDirException: public std::exception{
	public:
		virtual const char* what() const throw();
};

#endif