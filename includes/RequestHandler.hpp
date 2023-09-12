#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

# include "Server.hpp"
# include "serverUtils.hpp"

struct requestData;
struct locationInfo;

int handleGet(requestData &request, locationInfo &location ,std::string &response, serverConf &conf);
int handlePost(requestData &request, locationInfo &location ,std::string &response, serverConf &conf);
int handlePut(requestData &request, locationInfo &location ,std::string &response, serverConf &conf);
int handleHead(requestData &request, locationInfo &location ,std::string &response, serverConf &conf);
int handleDelete(requestData &request, locationInfo &location ,std::string &response, serverConf &conf);

#endif