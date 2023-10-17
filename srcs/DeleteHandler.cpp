#include "RequestHandler.hpp"

int handleDelete(requestData &request, locationInfo &location ,std::string &response, serverConf &conf){
	if (request.file_path.empty()){
		response = handleError(404, conf);
		return (404);
	}
	std::string path = location.root + "/" + request.file_path;
	if (remove(path.c_str()) != 0){
		response = handleError(404, conf);
		return (404);
	}
	else{
		response = "";
		return (204);
	}
}