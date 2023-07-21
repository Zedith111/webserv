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

int		checkMethod(std::string &method, std::vector<std::string> &limit_except){
	if (std::find(limit_except.begin(), limit_except.end(), method) == limit_except.end())
		return (-1);
	else if (method == "GET")
		return (GET);
	else if (method == "POST")
		return (POST);
	else if (method == "HEAD")
		return (HEAD);
	else if (method == "DELETE")
		return (DELETE);
	else
		return (-1);
}

std::string getStatusHeader(int status_code){
	// std::string status_header;

	// status_header += "HTTP/1.1 ";
	// switch (status_code)
	// {
	// case /* constant-expression */:
	// 	/* code */
	// 	break;
	
	// default:
	// 	break;
	// }

	// //"HTTP/1.1 404 Not Found\r\n\r\n";
	(void) status_code;
	return ("");
}