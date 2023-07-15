/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/11 11:31:24 by zah               #+#    #+#             */
/*   Updated: 2023/07/11 11:40:37 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "serverUtils.hpp"

int	parseHttpHeader(std::string &msg){
	std::string header_end = "\r\n\r\n";
	if (msg.find(header_end) == std::string::npos)
		return (0);
	return (1);
}