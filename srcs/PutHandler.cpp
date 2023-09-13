#include "RequestHandler.hpp"

/**
 * @brief PUT request is handle similar to POST request.
 * The only difference is that PUT will overwrite the file if it already exists, and return 200 when overwrite happen.
 */
int handlePut(requestData &request, locationInfo &location ,std::string &response, serverConf &conf){
	if (location.max_body_size_set == 1){
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
			res = handleBoundaryUpload(request, location, 1);
		else
			res = handleNormalUpload(request, location, 1);
	}
	else
		res = handleNormalUpload(request, location, 1);

	if (res == 0){
		response = handleError(500, conf);
		return (500);
	}
	if (res == 1){
		std::cout << COLOR_GREEN << "File created" << COLOR_RESET << std::endl;
		response = "";
		return (201);
	}
	else{
		response = "";
		return (200);
	}
}