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
 * @brief Check whether a request is a CGI request or not base on the server.
 * Will also set the file_path to be the path of the cgi script.
 */
int checkCGIRequest(std::string &path, serverConf &server, requestData &request){
	std::string::size_type extension_pos = path.find_last_of(".");
	if (extension_pos == std::string::npos){
		return (0);
	}
	std::string::size_type query_string_pos = path.find_last_of("?");
	std::string extension = path.substr(extension_pos, query_string_pos - extension_pos);
	if (extension.find("/") != std::string::npos)
		extension = extension.substr(0, extension.find("/"));

	std::multimap<std::string, std::string>::iterator it = server.cgi_map.find(extension);
	if (it == server.cgi_map.end()){
		std::cout << COLOR_RED << "Error: cgi extension not in map" << COLOR_RESET << std::endl;
		return (0);
	}
	request.interpretor = it->second;
	request.file_path = path.substr(0, extension_pos) + extension;
	return (1);
}

/**
 * @brief Parse the body of a POST or PUT request with multipart/form-data encoding. Separate
 * the data and put into a vector of formData struct.
 */
formData	parseMultipartFormData(std::string &body, std::string &boundary){
	formData form_data;
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
		}
	}
	return (form_data);
}

/**
 * @brief Create or overwrite a file with the data provided.
 */
int 		storeFile(std::string &file_path, std::string &data, int overwrite){
	std::ofstream file;
	if (overwrite == 1)
		file.open(file_path.c_str(), std::ios::trunc);
	else
		file.open(file_path.c_str(), std::ios::app);
	if (!file.is_open()){
		std::cout << "Error opening file: " << file_path << std::endl;
		return (0);
	}
	file << data;
	file.close();
	return (1);
}


/**
 * @brief Return the correct error page based on error code and server config
 */
std::string handleError(int error_code, serverConf &conf){
	std::string ret = conf.error_pages[error_code];
	std::ifstream file(ret.c_str());
	std::stringstream buffer;
	buffer << file.rdbuf();
	file.close();
	return (buffer.str());
}


int generateAutoindex(serverConf &conf, requestData &request, std::string &directory_path, std::string &response){
	std::string whole_route = request.route + request.file_path;
	if (checkIsDirectory(directory_path) <= 0){
		std::cout << COLOR_RED << "Error. Autoindex called for non-directory" << COLOR_RESET << std::endl;
		response = handleError(403, conf);
		return (403);
	}
	response += "<html>\n<head>\n<title>Index of " + whole_route + "</title>\n</head>\n<body>\n<h1>Index of " + whole_route + "</h1>\n";
	response += "<table>\n<tr>\n<th>Name</th>\n<th>Last Modified</th>\n<th>Size</th>\n</tr>\n";
	try{
		response += printDirectory(whole_route, directory_path);
	}
	catch(const std::exception &e){
		std::cout << COLOR_RED << "Error. Unable to open directory " << directory_path << COLOR_RESET << std::endl;
		response = handleError(500, conf);
		return (500);
	}
	response += "</table></body>\n</html>";
	return (200);
}

int handleBoundaryUpload(requestData &request, locationInfo &location, int overwrite){
	std::string upload_dir;

	if (location.upload_path.empty())
		upload_dir = location.root;
	else
		upload_dir = location.upload_path;
	if (checkIsDirectory(upload_dir) != 1){
		std::cout << COLOR_RED << "Error. Upload directory not found" << COLOR_RESET << std::endl;
		return (0);
	}
	DIR *dir = opendir(upload_dir.c_str());
	if (dir == NULL){
		std::cout << COLOR_RED << "Error. Unable to open directory " << upload_dir << COLOR_RESET << std::endl;
		return (0);
	}
	std::string::size_type start_pos = request.header.find("boundary=");
	std::string boundary = "--" + request.header.substr(start_pos + 9, request.header.find("\r\n", start_pos) - start_pos - 9);
	formData form_data = parseMultipartFormData(request.body, boundary);
	std::string whole_path = upload_dir + "/" + form_data.filename;
	int mult = 1;
	if (access(whole_path.c_str(), F_OK) == 0){
		if (overwrite == 0){
			std::cout << COLOR_RED << "Error. File already exist" << COLOR_RESET << std::endl;
			return (0);
		}
		else{
			std::cout << COLOR_YELLOW << "File already exist. Overwriting..." << COLOR_RESET << std::endl;
			mult = 2;
		}
	}
	int ret = storeFile(whole_path, form_data.data, overwrite);
	closedir(dir);
	return (ret * mult);
}

/**
 * @brief Upload the file to respective directory when the content type is not multipart/form-data.
 * Return 1 if successful create, 2 when successful overwrite, 0 if fail.
 */
int handleNormalUpload(requestData &request, locationInfo &location, int overwrite){	
	std::string upload_dir;
	
	if (location.upload_path.empty())
		upload_dir = location.root;
	else
		upload_dir = location.upload_path;
	std::cout << COLOR_CYAN << "Upload " << request.file_path << " to " << upload_dir << COLOR_RESET << std::endl;
	if (checkIsDirectory(upload_dir) != 1){
		std::cout << COLOR_RED << "Error. Upload directory not found" << COLOR_RESET << std::endl;
		return (0);
	}
	DIR *dir = opendir(upload_dir.c_str());
	if (dir == NULL){
		std::cout << COLOR_RED << "Error. Unable to open directory " << upload_dir << COLOR_RESET << std::endl;
		return (0);
	}
	std::string whole_path = upload_dir + request.file_path;
	int mult = 1;
	if (access(whole_path.c_str(), F_OK) == 0){
		if (overwrite == 0){
			std::cout << COLOR_RED << "Error. File already exist" << COLOR_RESET << std::endl;
			return (0);
		}
		else{
			std::cout << COLOR_YELLOW << "File already exist. Overwriting..." << COLOR_RESET << std::endl;
			mult = 2;
		}
	}
	int ret = storeFile(whole_path, request.body, overwrite);
	closedir(dir);
	return (ret * mult);
}

std::string processChunk(std::string &body){
	std::stringstream res;
	std::istringstream iss(body);
	std::string line;

	while (std::getline(iss, line))
	{
		size_t chunkSize;
		std::stringstream(line) >> std::hex >> chunkSize;
		if (chunkSize == 0)
			break ;
		
		std::vector<char> chunkData(chunkSize);
		iss.read(chunkData.data(), chunkSize);
		res.write(chunkData.data(), chunkSize);
		iss.ignore(2);
	}
	return (res.str());
}