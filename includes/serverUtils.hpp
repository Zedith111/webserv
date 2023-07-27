/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverUtils.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/11 11:25:07 by zah               #+#    #+#             */
/*   Updated: 2023/07/11 11:32:33 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERUTILS_HPP
# define SERVERUTILS_HPP

# include "Webserv.hpp"
# include "Server.hpp"

# include <algorithm>

int				checkMethod(std::string &method, std::vector<std::string> &limit_except);


#endif