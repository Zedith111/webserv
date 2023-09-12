#include "RequestHandler.hpp"

/**
 * @brief Put request is handle similar to oist request.
 * The only difference is that PUT will overwrite the file if it already exists, and return 200 when overwrite happen.
 */
int handlePut(requestData &request, locationInfo &location ,std::string &response, serverConf &conf){
	(void)request;
	(void)location;
	(void)response;
	(void)conf;
	std::cout << "handle Put" << std::endl;
	std::cout << "file path: " << request.file_path << std::endl;
	std::cout << "Upload store: " << location.upload_path << std::endl;

	return 0;
}