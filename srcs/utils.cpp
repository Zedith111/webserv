/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/24 13:14:45 by zah               #+#    #+#             */
/*   Updated: 2023/08/17 14:53:22 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

/**
 * @brief Check if the path is a directory
 * Return 1 if is directory, 0 if is a file, -1 if error
 */
int	checkIsDirectory(std::string &path){
	struct stat file_info;
	int res = stat(path.c_str(), &file_info);
	if (res < 0){
		path = "." + path;
		res = stat(path.c_str(), &file_info);
		if (res < 0){
			return (-1);
		}
	}
	if (S_ISDIR(file_info.st_mode)) {
		return (1);
	} else {
		return (0);
	}
}

std::string intToString(int num){
	std::stringstream ss;
	ss << num;
	return (ss.str());
}

std::ostream&	operator<<(std::ostream& os, const serverConf& obj){
	os << "Host name: "  << obj.host << std::endl;
	os << "Port: " ;
	for (size_t i = 0; i < obj.port_number.size(); i++){
		os << obj.port_number[i] << ",";
	}
	os << std::endl;
	os << "Server name: " << obj.server_name << std::endl;
	os << "Root: " << obj.root << std::endl;
	os << "Error Pages: " << std::endl;
	std::map<int, std::string>::const_iterator error_iter;
	for (error_iter = obj.error_pages.begin(); error_iter != obj.error_pages.end(); error_iter++){
		os << "\t" << error_iter->first << ": " << error_iter->second << std::endl;
	}
	std::map<std::string, locationInfo *>::const_iterator location_iter;
	for (location_iter = obj.locations.begin(); location_iter != obj.locations.end(); location_iter++){
		os << "Route: " << location_iter->first << std::endl;
		os << "\tRoot: " << location_iter->second->root << std::endl;
		os << "\tIndex: " << location_iter->second->index << std::endl;
		os << "\tAutoindex: " << location_iter->second->autoindex << std::endl;
		os << "\tLimit Except: ";
		if (location_iter->second->limit_except.size() == 0){
			os << "None";
		}
		for (size_t j=0; j < location_iter->second->limit_except.size(); j++){
				os << location_iter->second->limit_except[j] << ", ";
		}
		os << std::endl;
	}
	os << "\t\tEnd of Config File";
	return (os);
}

/**
 * @brief Route refer to the route that pass to the server, file_path refer to the actual path of the file in the harddrive
 */
std::string 	printDirectory(std::string &route, std::string &file_path){
	DIR *dir = opendir(file_path.c_str());
	if (dir == NULL){
		throw unableToOpenDirException();
	}
	std::string res;
	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL){
		std::string file_name = entry->d_name;
		if (file_name == "." || file_name == "..")
			continue ;
		std::string whole_path = file_path + "/" + file_name;
		std::string whole_route = route + "/" + file_name;
		res += "<tr>\n";
		res += "<td><a href=\"" + whole_route + "\">";
		res += file_name;
		std::cout << "File name: " << file_name << std::endl;
		res += "</a>\n</td>";
		struct stat file_stat;
		if (stat(whole_path.c_str(), &file_stat) != 0){
			throw unableToOpenDirException();
		}
		res += "\n<td>" + intToString(file_stat.st_size) + " bytes</td>";
		res += "\n<td>" + std::string(ctime(&file_stat.st_mtime)) + "</td>";
		res += "<br>\n</tr>";
		if (checkIsDirectory(whole_path) == 1){
			res += printDirectory(whole_route, whole_path);
		}
	}
	closedir(dir);
	return (res);
}


const char *unableToOpenDirException::what() const throw()
{
	return ("unable to open directory");
}
