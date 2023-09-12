#include "RequestHandler.hpp"

/**
 * @brief Read the file path of a target location and return the body of response
 * Handle get request, will follow order below
 * 		- Search for specific file eg. /index.html. If not found will sent 404
 * 		- Handle redirection
 * 		- Use index parameter
 * 		- Use autoindex parameter
 */
int handleGet(requestData &request, locationInfo &location, std::string &response, serverConf &conf){
	if (request.file_path.empty() && location.index.empty() && location.redirect_address.empty()){
		if (location.autoindex == true)
			return (generateAutoindex(conf, request.route, location.root, response));
		else{
			response = handleError(404, conf);
			return (404);
		}
	}
	std::string whole_path;
	if (!request.file_path.empty()){
		whole_path = location.root + request.file_path;
		if (checkIsDirectory(whole_path) == 1){
			std::map<std::string, locationInfo *>::iterator it;
			it = conf.locations.find(request.route);
			if (it != conf.locations.end()){
				if (it->second->autoindex == true)
					return (generateAutoindex(conf, request.route, whole_path, response));
				else{
					response = handleError(404, conf);
					return (404);
				}
			}
		}
	}
	else if (!location.redirect_address.empty()){
		response = location.redirect_address;
		return (301);
	}
	else{
		whole_path = location.root + "/" + location.index;
	}
	std::ifstream file(whole_path.c_str());
	if (!file.is_open()){
		std::cout << COLOR_RED << "Error. File not found. " << whole_path << COLOR_RESET << std::endl;
		response = handleError(404, conf);
		return (404);
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	response = buffer.str();
	file.close();
	return (200);
}