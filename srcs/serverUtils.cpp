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

/**
 * @brief Check whether a request is complete.
 * First check for the end of header.
 * 
 * Reminder:
 */
int	checkReceive(std::string &msg, requestData &request){
	std::string header_end = "\r\n\r\n";
	if (msg.find(header_end) == std::string::npos)
		return (0);
	if (msg.find("Transfer-Encoding: chunked") != std::string::npos){
		if (msg.find("\r\n0\r\n\r\n") == std::string::npos)
			return (0);
	}
	if (msg.find("Content-Length: ") != std::string::npos){
		//add to body
		size_t 
	}
	return (1);
}