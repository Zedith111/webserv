#include "RequestHandler.hpp"

int handleHead(requestData &request, locationInfo &location ,std::string &response, serverConf &conf){
	std::string path;
	if (!request.file_path.empty())
		path = location.root + request.file_path;
	else
		path = location.root + "/" + location.index;
	std::ifstream file(path.c_str());
	if (!file.is_open()){
		std::cout << COLOR_RED << "Error: File not found. " << path << COLOR_RESET << std::endl;
		response = handleError(404, conf);
		return (404);
	}
	else{
		response = "";
		file.close();
		return (200);
	}
}