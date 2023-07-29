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
	//Check if  limit_except present;
	// if (limit_except.size() == 0){
	// 	std::cout << "Empty" << std::endl;
	// }
	std::cout << "Method: " << method << std::endl;
	if (std::find(limit_except.begin(), limit_except.end(), method) == limit_except.end()){
		std::cout << method <<"Not found" << std::endl;
		return (-1);
	}
	else if (method == "GET")
		return (GET);
	else if (method == "POST")
		return (POST);
	else if (method == "PUT")
		return (PUT);
	else if (method == "HEAD")
		return (HEAD);
	else if (method == "DELETE")
		return (DELETE);
	else
		return (-1);
}