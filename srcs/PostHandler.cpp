#include "RequestHandler.hpp"

int handlePost(requestData &request, locationInfo &location ,std::string &response, serverConf &conf){
	if (location.max_body_size_set == 1){
		std::cout << "body size: " << request.body.size() << std::endl;
		if (request.body.size() > location.max_body_size){
			response = handleError(413, conf);
			return (413);
		}
	}

	std::string::size_type content_type_pos = request.header.find("Content-Type");
	int res;
	if (content_type_pos != std::string::npos){
		std::string content_type = request.header.substr(content_type_pos + 14, request.header.find("\r\n", content_type_pos) - content_type_pos - 14);
		if (content_type.find("multipart/form-data") != std::string::npos)
			res = handleBoundaryUpload(request, location, 0);
		else
			res = handleNormalUpload(request, location, 0);
	}
	else
		res = handleNormalUpload(request, location, 0);

	if (res == 0){
		if (TEST == 1)
			return (200);
		response = handleError(409, conf);
		return (409);
	}
	else{
		request.status_code = 201;
		response = "";
		return (201);
	}
}