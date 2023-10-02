#include "RequestHandler.hpp"

/**
 * @brief Read a file and write to response. Return 0 if failed
 */
void read_file(std::string file_path, std::string &response){
	std::ifstream file(file_path.c_str());
	if (!file.is_open()){
		throw requestException("Error. File not found. " + file_path);
		return ;
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	response = buffer.str();
	file.close();
}

/**
 * @brief Read the file path of a target location and return the body of response
 * Handle get request, will follow order below
 * 		- Search for specific file, for example /test.pdf
 * 		- Search for present of index file
 * 		- Handle redirection
 * 		- Use autoindex parameter
 * If all not fufil, sent 404
 */
int handleGet(requestData &request, locationInfo &location, std::string &response, serverConf &conf){
	std::string whole_path;


	if (!request.file_path.empty()){
		std::string file_path = request.file_path;
		if (file_path[0] != '/'){
			file_path = "/" + file_path;
		}
		whole_path = location.root + file_path;
		int res = checkIsDirectory(whole_path);
		if (res == 1){
			if (!location.index.empty()){
				std::string index_path = whole_path + "/" + location.index;
				std::ifstream file(index_path.c_str());
				if (file.is_open()){
					try{
						//Found index in directory
						std::cout << "Found index in directory" << std::endl;
						read_file(index_path, response);
					}
					catch (std::exception &e){
						std::cout << COLOR_RED << e.what() << COLOR_RESET << std::endl;
						response = handleError(404, conf);
						return (404);
					}
					return (200);
				}
			}
			std::cout << "Index not found" << std::endl;
			if (TEST == 1){
				//If tester, return 404
				response = handleError(404, conf);
				return (404);
			}
			else {
				//If not tester, check the route have present of autoindex
				std::cout << "Subdirectory autoindex" << std::endl;
				if (location.autoindex == true)
					return (generateAutoindex(conf, request, whole_path, response));
				else{
					response = handleError(404, conf);
					return (404);
				}
			}
		}
		else {
			//File path is not directory, serve file
			try {
				read_file(whole_path, response);
			}
			catch (std::exception &e){
				response = handleError(404, conf);
				return (404);
			}
			return (200);
		}
	}
	else{
		if (!location.index.empty()){
			std::cout << "Serve location index" << std::endl;
			whole_path = location.root + "/" + location.index;
		}
		else if (!conf.default_index.empty()){
			std::cout << "Server default index" << std::endl;
			whole_path = conf.default_index;
		}
		else if (!location.redirect_address.empty()){
			std::cout << "Redirect" << std::endl;
			response = location.redirect_address;
			return (301);
		}
		else if (location.autoindex == true){
			std::cout << "Autoindex" << std::endl;
			return (generateAutoindex(conf, request, location.root, response));
		}
		else{
			response = handleError(404, conf);
			return (404);
		}
	}
	try {
		read_file(whole_path, response);
	}
	catch (std::exception &e){
		std::cout << COLOR_RED << e.what() << COLOR_RESET << std::endl;
		response = handleError(404, conf);
		return (404);
	}
	return (200);
}