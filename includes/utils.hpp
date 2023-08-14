/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 12:20:00 by zah               #+#    #+#             */
/*   Updated: 2023/08/14 15:28:20 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include "Webserv.hpp"

int	checkIsDirectory(std::string &path);
std::string intToString(int num);
std::ostream&	operator<<(std::ostream& os, const serverConf& obj);

#endif