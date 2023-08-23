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

METHOD	getMethod(std::string &method, std::vector<std::string> &limit_except){
	if (limit_except.size() > 0){
		if (std::find(limit_except.begin(), limit_except.end(), method) == limit_except.end()){
			return (NOT_ALLOWED);
		}
	}
	if (method == "GET")
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
		return (NOT_IMPLEMENTED);
}

/**
 * @brief Parse the body of a POST request with multipart/form-data encoding. Separate
 * the data and put into a vector of formData struct.
 */
std::vector<formData>	parseUpload(std::string &body, std::string &boundary){
	std::vector<formData>	formDataList;
	std::string::size_type current_pos = body.find(boundary);
	while (current_pos != std::string::npos){
		std::string::size_type next_pos = body.find(boundary, current_pos + boundary.length());
		if (next_pos == std::string::npos){
			break ;
		}

		std::string data = body.substr(current_pos, next_pos - current_pos);
		current_pos = next_pos;

		std::string::size_type disposition_pos = data.find("Content-Disposition: form-data;");
		if (disposition_pos != std::string::npos){
			formData form_data;

			std::string::size_type name_pos = data.find("name=\"", disposition_pos);
			if (name_pos != std::string::npos){
				name_pos += 6;
				std::string::size_type name_pos_end = data.find("\"", name_pos);
				form_data.name = data.substr(name_pos, name_pos_end - name_pos);
			}

			std::string::size_type filename_pos = data.find("filename=\"", disposition_pos);
			if (filename_pos != std::string::npos){
				filename_pos += 10;
				std::string::size_type filename_pos_end = data.find("\"", filename_pos);
				form_data.filename = data.substr(filename_pos, filename_pos_end - filename_pos);
			}
			form_data.data = data.substr(data.find("\r\n\r\n") + 4);
			formDataList.push_back(form_data);
		}
		
	}
	return (formDataList);
}

int	storeFile(std::string &directory_path, formData &form_data){
	std::string file_path = directory_path + "/" + form_data.filename;
	std::ofstream file(file_path.c_str());
	if (!file.is_open()){
		std::cout << "Error opening file: " << file_path << std::endl;
		return (0);
	}
	file << form_data.data;
	file.close();
	return (1);
}