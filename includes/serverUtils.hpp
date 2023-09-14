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

struct formData{
	std::string name;
	std::string filename;
	std::string data;
};

struct requestData;

METHOD		getMethod(std::string &method, std::vector<std::string> &limit_except);
int 		checkCGIRequest(std::string &path, serverConf &server, requestData &request);
formData	parseMultipartFormData(std::string &body, std::string &boundary);

int			storeFormData(std::string &directory_path, formData &form_data);
int 		storeFile(std::string &file_path, std::string &data, int overwrite);
int			checkFileExist(std::string &file_path);

std::string handleError(int error_code, serverConf &conf);
int generateAutoindex(serverConf &conf,std::string &route, std::string &file_path, std::string &response);

int handleNormalUpload(requestData &request, locationInfo &location, int overwrite);
int handleBoundaryUpload(requestData &request, locationInfo &location, int overwrite);

std::string processChunk(std::string &request);



#endif