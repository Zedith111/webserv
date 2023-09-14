/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/05 11:35:41 by zah               #+#    #+#             */
/*   Updated: 2023/07/09 12:58:57 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <unistd.h>
/**
 * @brief Server object constructor. Will initialize following
 * 1. Reason code and reason phrase 
 */
Server::Server(){
	// //Add reason phrase
	this->reason_phrases[200] = "OK";
	this->reason_phrases[201] = "Created";
	this->reason_phrases[204] = "No Content";
	this->reason_phrases[301] = "Moved Permanently";
	this->reason_phrases[400] = "Bad Request";
	this->reason_phrases[403] = "Forbidden";
	this->reason_phrases[404] = "Not Found";
	this->reason_phrases[405] = "Method Not Allowed";
	this->reason_phrases[409] = "Conflict";
	this->reason_phrases[413] = "Request Enity Too Large";
	this->reason_phrases[415] = "Unsupported Media Type";
	this->reason_phrases[500] = "Internal Server Error";
	this->reason_phrases[501] = "Not Implemented";
}

Server::~Server(){
	this->database.close();
}

/**
 * @brief Server initialization function. Loop through all server configuration and initialize them.
 * Return 0 if any error occur, 1 if success
 */
int	Server::init(std::vector<serverConf *> confs){
	struct addrinfo hints;
	struct addrinfo *res;
	this->confs = confs;

	if (DEBUG)
		std::cout << "\tInitializing server" << std::endl;
	
	//Open or create database used to store post data
	this->database.open("database.txt", std::ios::out);
	if (!this->database.is_open()){
		std::cerr << COLOR_RED << "Error. Unable to open database" << COLOR_RESET << std::endl;
		return (0);
	}

	//Get Address info
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	for (size_t i = 0; i < this->confs.size(); i ++){
		serverConf conf = *(this->confs[i]);
		for (size_t i = 0; i < conf.port_number.size(); i ++){
			int status = getaddrinfo(conf.host.c_str(), conf.port_number[i].c_str(), &hints, &res);
			if (status != 0){
				std::cerr << COLOR_RED << "Error: " << gai_strerror(status) << COLOR_RESET << std::endl;
				freeaddrinfo(res);
				return (0);
			}
			std::cout << COLOR_GREEN << "Get address info success for port " << conf.host << ":" << conf.port_number[i] << COLOR_RESET << std::endl;
			
			if (res->ai_next != NULL)
				std::cout << COLOR_YELLOW << "Warning: Multiple address detected. The program will only use the first one" << COLOR_RESET << std::endl;
			
			//Create Socket
			int socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
			if (socket_fd < 0){
				std::cerr << COLOR_RED << "Error. Unable to create socket for port " << conf.port_number[i] << strerror(errno) << std::endl;
				freeaddrinfo(res);
				return (0);
			}
			this->sockets_port[socket_fd] = conf.port_number[i];
			std::cout << COLOR_GREEN << "Socket " << socket_fd << " created for port " << conf.port_number[i] << COLOR_RESET << std::endl;
			this->servers[socket_fd] = conf;

			int reuse = 1;
			if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0){
				std::cerr << COLOR_RED << "Error. Unable to set socket option for port " << conf.port_number[i] << strerror(errno) << std::endl;
				freeaddrinfo(res);
				return (0);
			}

			//Bind Socket
			if (bind(socket_fd, res->ai_addr, res->ai_addrlen) < 0){
				std::cerr << COLOR_RED << "Error. Unable to bind socket for port " << conf.port_number[i]  << ". " << strerror(errno) << std::endl;
				freeaddrinfo(res);
				return (0);
			}
			std::cout << COLOR_GREEN << "Socket " << socket_fd << " binded for port " << conf.port_number[i] << COLOR_RESET << std::endl;

			//Listen Socket
			if (listen(socket_fd, SOMAXCONN) < 0){
				std::cerr << COLOR_RED << "Error. Unable to listen socket for port " << conf.port_number[i] << strerror(errno) << std::endl;
				freeaddrinfo(res);
				return (0);
			}
			std::cout << COLOR_GREEN << "Socket " << socket_fd << " listening for port " << conf.port_number[i] << COLOR_RESET << std::endl;
			std::cout << COLOR_GREEN << "Server " << conf.host << ":" << conf.port_number[i] << " initialize successfully" << COLOR_RESET << std::endl;
		}
	}
	freeaddrinfo(res);	
	return (1);
}

/**
 * @brief Run a infinite loop which handle connection and request. The process are as following
 * 	1) Check the fd return by select
 * 		- if the fd equal to one of the server socket, it is a new connection
 * 		- else, it is a ongoing connection
 * 	2) If it is a new connection, accept the connection and add it to the read_fd
 * 	3) If it is a ongoing connection, handle the request
 */
void	Server::run(){
	fd_set 			read_ready_fd, write_ready_fd;
	struct timeval	timeout;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	FD_ZERO(&this->read_fd);
	FD_ZERO(&this->write_fd);
	for (std::map<int, serverConf>::iterator it = this->servers.begin(); it != this->servers.end(); it++){
		FD_SET(it->first, &this->read_fd);
	}
	while (1){
		memcpy(&read_ready_fd, &this->read_fd, sizeof(fd_set));
		memcpy(&write_ready_fd, &this->write_fd, sizeof(fd_set));

		int select_val = select(FD_SETSIZE, &read_ready_fd, &write_ready_fd, NULL, &timeout);
		if (select_val < 0){
			std::cout << COLOR_RED << "Error. select failed. " << strerror(errno) << COLOR_RESET << std::endl;
			std::cout << "Trying to reset" << std::endl;
			for (std::map<int, requestData>::iterator it = this->client_requests.begin(); it != this->client_requests.end(); it ++){
				close(it->first);
			}
			this->client_requests.clear();
			this->client_responses.clear();
			FD_ZERO(&this->read_fd);
			for (std::map<int, serverConf>::iterator it = this->servers.begin(); it != this->servers.end(); it++){
				FD_SET(it->first, &this->read_fd);
			}
		}
		//no fd ready, move to next loop
		if (select_val == 0)
			continue;

		//Reading Request
		for (int i = 0; i < FD_SETSIZE; i++){
			if (!FD_ISSET(i, &read_ready_fd))
				continue ;
			int is_new_socket = 0;
			//If equal to one of server socket, it is new connection
			if (this->servers.find(i) != this->servers.end()){
				int new_socket = acceptNewConnection(i);
				if (new_socket < 0)
					continue ;
				FD_SET(new_socket, &this->read_fd);
				is_new_socket = 1;
				break ;
			}
			if (!is_new_socket){
				int handle_ret = handleConnection(i);
				if (handle_ret == -1){
					FD_CLR(i, &this->read_fd);
					this->client_requests.erase(i);
				}
				if (handle_ret == 1){
					handleRequest(i);
					FD_SET(i, &this->write_fd);
					FD_CLR(i, &this->read_fd);
				}
				break ;
			}
		}
		//Writing Response
		for(int i = 0; i < FD_SETSIZE; i ++){
			if (!FD_ISSET(i, &write_ready_fd))
				continue ;
			sendResponse(i);
			std::cout << "Closing socket " << i << std::endl;
			FD_CLR(i, &this->write_fd); 
			break ;
		}
	}
}

/**
 * @brief Accept new connection and map the client to correct server. Return new socket fd
 * when success, -1 when error
 */
int	Server::acceptNewConnection(int socket_fd){
	int new_socket = accept(socket_fd, NULL, NULL);
	if (new_socket < 0){
		std::cout << COLOR_RED << "Error: Accept failed " << strerror(errno) << COLOR_RESET << std::endl;
		close(socket_fd);
		return (-1);
	}
	this->client_requests[new_socket].server_fd = socket_fd;
	this->client_requests[new_socket].port = this->sockets_port[socket_fd];
	this->client_requests[new_socket].status_code = 0;
	if (DEBUG){
		std::cout << "New client joined at socket " << new_socket << std::endl;
		std::cout << "Join at port " << this->sockets_port[socket_fd] << std::endl;
	}
	fcntl(new_socket, F_SETFL, O_NONBLOCK);
	return (new_socket);
}

/**
 * @brief Receive the all message sent by client. Return -1 when client close connection or error
 *  1 when completed, 0 when ongoing
 */
int	Server::handleConnection(int socket_fd){
	int		bytes_read;
	char	buffer[BUFFER_SIZE];

	memset(buffer, 0, BUFFER_SIZE);
	bytes_read = recv(socket_fd, buffer, BUFFER_SIZE, 0);

	if (bytes_read == 0){
		std::cout << "Client hangout at " << socket_fd << std::endl;
		close(socket_fd);
		return (-1);
	}
	if (bytes_read < 0){
		std::cout << COLOR_RED << "Error. recv failed at " << socket_fd << ". " << strerror(errno) << COLOR_RESET <<  std::endl;

		this->client_responses[socket_fd] = handleError(500, this->servers[this->client_requests[socket_fd].server_fd]);
		this->client_requests[socket_fd].status_code = 500;
		return (-1);
	}
	while (bytes_read > 0){
		std::cout << COLOR_GREEN << "Receiving " << bytes_read << COLOR_RESET << "\r\n";
		// std::cout.flush();
		this->client_requests[socket_fd].whole_request.append(buffer, bytes_read);
		memset(buffer, 0, BUFFER_SIZE);
		bytes_read = recv(socket_fd, buffer, BUFFER_SIZE, 0);
	}
	return (checkReceive(this->client_requests[socket_fd].whole_request));
}

/**
 * @brief Evaluate the request header and body. The order are as following
 * 	1. Check if is cgi request
 *  2. Check if is a direct file request
 *  3. Check if in the location path
 * Write the response body to client_responses
 */
void		Server::handleRequest(int socket_fd){
	if (this->client_requests[socket_fd].status_code != 0)
		return ;
	std::string header = this->client_requests[socket_fd].whole_request.substr(0, this->client_requests[socket_fd].whole_request.find("\r\n\r\n"));
	std::string body = this->client_requests[socket_fd].whole_request.substr(this->client_requests[socket_fd].whole_request.find("\r\n\r\n") + 4);
	size_t contentLength = body.length();
	this->client_requests[socket_fd].header = header;
	this->client_requests[socket_fd].body = body;
	this->client_requests[socket_fd].contentLength = contentLength;

	int pos = header.find(' ');
	std::string method = header.substr(0, pos);
	pos ++;
	std::string route = header.substr(pos, header.find(' ', pos) - pos);
	this->client_requests[socket_fd].route = route;

	int	server_fd = this->client_requests[socket_fd].server_fd;
	if (DEBUG){
		std::cout << COLOR_CYAN << "[" << std::endl;
		std::cout << "Request Header: " << std::endl << header << std::endl;
		std::cout << "Request Body: " << std::endl << body << std::endl;
		std::cout << "Method: " << method << std::endl;
		std::cout << "Route: " << route << std::endl;
		std::cout << "Route Root: " << this->servers[server_fd].locations[route]->root << std::endl;
		std::cout << "Route Index: " << this->servers[server_fd].locations[route]->index << std::endl;
		std::cout << "]" << COLOR_RESET << std::endl;
	}
	if (!checkHost(this->client_requests[socket_fd].header, this->servers[server_fd].server_name)){
		std::cout << COLOR_RED << "Error. Host and server name does not match" << COLOR_RESET << std::endl;
		this->client_responses[socket_fd] = handleError(400,this->servers[this->client_requests[socket_fd].server_fd]);
		this->client_requests[socket_fd].status_code = 400;
		return ;
	}

	if (route == "/favicon.ico"){
		std::cout << COLOR_YELLOW << "Favicon requested" << COLOR_RESET << std::endl;
		this->client_responses[socket_fd] = handleError(404, this->servers[this->client_requests[socket_fd].server_fd]);
		this->client_requests[socket_fd].status_code = 404;
		return ;
	}

	if (header.find("Transfer-Encoding: chunked") != std::string::npos){
		this->client_requests[socket_fd].body = processChunk(this->client_requests[socket_fd].whole_request);
		// std::cout << "After chunked Body: " << this->client_requests[socket_fd].body	<< std::endl;
	}

	//Check if the path is valide. Will perform two check
	//	First check for the path is exact match with what present in the location
	//	If no, check for prefix match, and extract the path after the prefix
	//If not found, return 404
	if (this->servers[server_fd].locations.find(route) == this->servers[server_fd].locations.end()){
		std::string file_path = checkDirectoryRoute(server_fd, route);
		if (file_path.empty()){
			std::cout << COLOR_RED <<  "Error. Route: " << route <<  " not found"  << COLOR_RESET << std::endl;
			this->client_responses[socket_fd] = handleError(404, this->servers[this->client_requests[socket_fd].server_fd]);
			this->client_requests[socket_fd].status_code = 404;
			return ;
		}
		else{
			this->client_requests[socket_fd].route = route;
			this->client_requests[socket_fd].file_path = file_path;
		}
	}

	if(checkCGIRequest(this->client_requests[socket_fd].file_path, this->servers[server_fd], this->client_requests[socket_fd])){
		if (method != "GET" && method != "POST"){
			std::cout << COLOR_RED << "Error. Method " << method << " is not allowed for cgi"  << COLOR_RESET << std::endl;
			this->client_responses[socket_fd] = handleError(405, this->servers[this->client_requests[socket_fd].server_fd]);
			this->client_requests[socket_fd].status_code = 405;
			return ;
		}
		if (method == "GET")
			this->client_requests[socket_fd].method = GET;
		else
			this->client_requests[socket_fd].method = POST;
		this->client_requests[socket_fd].is_cgi = 1;
		this->client_requests[socket_fd].status_code = handleCGI(
			this->client_requests[socket_fd], *(this->servers[server_fd].locations[route]),
			this->client_responses[socket_fd],
			this->servers[server_fd]);
		return ;
	}

	//Check for the content-length exceed the limit
	if (this->servers[server_fd].locations[route]->max_body_size_set == 1){
		if (this->client_requests[socket_fd].contentLength > this->servers[server_fd].locations[route]->max_body_size){
			std::cout << COLOR_RED << "Error. Content-Length exceed limit" << COLOR_RESET << std::endl;
			this->client_responses[socket_fd] = handleError(413, this->servers[this->client_requests[socket_fd].server_fd]);
			this->client_requests[socket_fd].status_code = 413;
			return ;
		}
	}
	
	//Check if Method can be handle
	METHOD request_method = getMethod(method, this->servers[server_fd].locations[route]->limit_except);
	this->client_requests[socket_fd].method = request_method;
	int method_int = static_cast<int>(request_method);
	if (method_int < 0){
		if (method_int == -2){
			std::cout << COLOR_RED << "Error. Method " << method << " is not allowed"  << COLOR_RESET << std::endl;
			this->client_responses[socket_fd] = handleError(405, this->servers[this->client_requests[socket_fd].server_fd]);
			this->client_requests[socket_fd].status_code = 405;
			return ;
		}
		if (method_int == -1){
			std::cout << COLOR_RED << "Error. Unknown method. " << method << COLOR_RESET << std::endl;
			this->client_responses[socket_fd] = handleError(501, this->servers[this->client_requests[socket_fd].server_fd]);
			this->client_requests[socket_fd].status_code = 501;
			return ;
		}
	}

	typedef int (*func)(requestData &, locationInfo &, std::string &, serverConf &);
	func methods[METHOD_COUNT] = {&handleGet, &handlePost, &handlePut, &handleHead, &handleDelete};
	this->client_requests[socket_fd].status_code = (*methods[method_int])
													(this->client_requests[socket_fd],
													*(this->servers[server_fd].locations[route]),
													this->client_responses[socket_fd],
													this->servers[server_fd]);
}

/**
 * @brief Add header to the response body and sent it.
 */
void	Server::sendResponse(int socket_fd){
	std::string res;
	if (!this->client_requests[socket_fd].is_cgi){
		int status_code = this->client_requests[socket_fd].status_code;
		std::cout << COLOR_MAGENTA << "Sending response " << status_code << " to socket " << socket_fd << COLOR_RESET << std::endl;
		res = "HTTP/1.1 ";
		res += intToString(status_code);
		res += " ";
		res += this->reason_phrases[status_code];
		res += "\r\n";
		if (status_code == 301){
			res += "Location: ";
			res += this->client_responses[socket_fd];
			res += "\r\n\r\n";
		}
		else{
			res += "Content-Type: text/html\r\n";
			res += "Content-Length: ";
			res += intToString(this->client_responses[socket_fd].length());
			res += "\r\n\r\n";
		}
	}
	res += this->client_responses[socket_fd];
	int byteSend = send(socket_fd, res.c_str(), res.length(), 0);
	if (byteSend < 0)
		std::cout << COLOR_RED << "Error. Send failed at " << socket_fd << strerror(errno) << COLOR_RESET << std::endl;
	this->client_requests.erase(socket_fd);
	this->client_responses.erase(socket_fd);
	std::cout << COLOR_MAGENTA << "Sent " << byteSend << " bytes to socket: " << socket_fd << COLOR_RESET << std::endl;
	close(socket_fd);
}

/**
 * @brief Check that all the data has been received.First look for end of header(\r\n\r\n).
 * Then check for transder encoding chunked(0\r\n\r\n) and content length
 */
int	Server::checkReceive(std::string &msg){
	std::string header_end = "\r\n\r\n";

	if (msg.find(header_end) == std::string::npos){
		return (0);
	}
	if (msg.find("Transfer-Encoding: chunked") != std::string::npos){
		if (msg.find("0\r\n\r\n") == std::string::npos)
			return (0);
		else
			return (1);
	}
	if (msg.find("Content-Length: ") != std::string::npos){
		size_t	value_pos = msg.find("Content-Length: ");
		size_t	end_pos = msg.find("\r\n", value_pos);
		std::string content_length = msg.substr(value_pos + 16, end_pos - value_pos - 16);
		std::stringstream ss(content_length);
		size_t length;
		ss >> length;
		std::string body = msg.substr(msg.find("\r\n\r\n") + 4);
		return (body.length() == length);
	}
	return (1);
}

/**
 * @brief Find the host name parameter and match it with the server name parameter
 */
int Server::checkHost(std::string &header, std::string &server_name){
	std::string::size_type pos = header.find("Host: ");
	if (pos == std::string::npos){
		std::cout << COLOR_RED << "Error. Host parameter not found" << COLOR_RESET << std::endl;
		return (0);
	}
	pos += 6;
	std::string::size_type end_pos = header.find("\r\n", pos);
	std::string host = header.substr(pos, end_pos - pos);
	pos = host.find(":");
	if (pos != std::string::npos)
		host = host.substr(0, pos);
	if (host == server_name)
		return (1);
	
	std::ifstream host_file("/etc/hosts");
	std::string line;
	if (host_file.is_open()){
		while (std::getline(host_file, line)) {
			if (line.find(host) != std::string::npos){
        		if (line.find(server_name) != std::string::npos)
					return (1);
			}
    	}
	}
	return (0);
}

//Find extension
//Find intepretor
//differntiate between get and post
//set up env
// std::string Server::handleCGI(int &client_fd, locationInfo *location){
// 	std::cout << "HANDLE CGI" << std::endl;

// 	std::string cgi_path = location->root + "/" + this->client_requests[client_fd].file_path;
// 	std::string interpretor = this->client_requests[client_fd].interpretor;
	
// 	std::cout << "Interpretor: " << interpretor << std::endl;
// 	std::cout << "CGI Path: " << cgi_path << std::endl;

// 	if (access(cgi_path.c_str(), R_OK| X_OK) != 0){
// 		std::cout << COLOR_RED << "Error: Unable to access cgi script: " << cgi_path << COLOR_RESET << std::endl;
// 		this->client_requests[client_fd].status_code = 500;
// 		return (handleError(500, this->servers[this->client_requests[client_fd].server_fd]));
// 	}

// 	if (access(interpretor.c_str(), R_OK| X_OK) != 0){
// 		std::cout << COLOR_RED << "Error: Unable to access interpretor: " << interpretor << COLOR_RESET << std::endl;
// 		this->client_requests[client_fd].status_code = 500;
// 		return (handleError(500, this->servers[this->client_requests[client_fd].server_fd]));
// 	}

// 	int stdin = dup(STDIN_FILENO);
// 	int stdout = dup(STDOUT_FILENO);

// 	//tempIn is used to store the duplicate script, tempOut is used to store the output
// 	FILE *tempIn = std::tmpfile();
// 	FILE *tempOut = std::tmpfile();
// 	int fdIn = fileno(tempIn);
// 	int fdOut = fileno(tempOut);
// 	std::string res;
	
// 	//Write the script to tempIn
// 	FILE *script = fopen(cgi_path.c_str(), "r");
// 	if (script == NULL){
// 		std::cout << COLOR_RED << "Error: Unable to open cgi script: " << cgi_path << COLOR_RESET << std::endl;
// 		this->client_requests[client_fd].status_code = 500;
// 		return (handleError(500, this->servers[this->client_requests[client_fd].server_fd]));
// 	}

// 	char buffer[1024];
// 	int bytes_read;
// 	while ((bytes_read = std::fread(buffer, 1, 1024, script)) > 0){
// 		std::fwrite(buffer, 1, bytes_read, tempIn);
// 	}
// 	fclose(script);
// 	std::rewind(tempIn);

// 	pid_t child = fork();
// 	if (child < 0){
// 		std::cout << COLOR_RED << "Error: Unable to fork" << COLOR_RESET << std::endl;
// 		fclose(tempIn);
// 		fclose(tempOut);
// 		this->client_requests[client_fd].status_code = 500;
// 		return (handleError(500, this->servers[this->client_requests[client_fd].server_fd]));
// 	}
// 	if (child == 0){
// 		dup2(fdIn, STDIN_FILENO);
// 		dup2(fdOut, STDOUT_FILENO);
// 		char *argv[3];
// 		argv[0] = const_cast<char *>(this->client_requests[client_fd].interpretor.c_str());
// 		argv[1] = strdup(this->client_requests[client_fd].file_path.c_str());
// 		argv[2] = NULL;

// 		execve(argv[0], argv, NULL);
// 	}
// 	else{
// 		int status;
// 		waitpid(child, &status, 0);
// 		char buffer[1024];
// 		std::rewind(tempOut);
// 		while ((bytes_read = std::fread(buffer, 1, 1024, tempOut)) > 0){
// 			res.append(buffer, bytes_read);
// 		}
// 	}
// 	dup2(stdin, STDIN_FILENO);
// 	dup2(stdout, STDOUT_FILENO);
// 	fclose(tempIn);
// 	fclose(tempOut);

// 	return (res);
// }

/**
 * @brief Called when no exact match found in route. Will check for prefix match and return the file path. 
 * Also modify the input argument to remove the file name from the path.
 * Return empty string if not found.
 */
std::string Server::checkDirectoryRoute(int server_fd, std::string &route){
	std::string file_path;

	std::map<std::string, locationInfo *>::iterator it;
	for (it = this->servers[server_fd].locations.begin(); it != this->servers[server_fd].locations.end(); it ++){
		const std::string &base_path = it->first;
		if (route.substr(0, base_path.length()) == base_path){
			file_path = route.substr(base_path.length());
		}
	}
	route = route.substr(0, route.length() - file_path.length());
	if (file_path[0] != '/'){
		file_path = "/" + file_path;
	}
	return (file_path);
}

std::string Server::handlePostText(int &client_fd){
	std::string content = this->client_requests[client_fd].body;
	this->database << content << std::endl;
	this->database.flush();
	this->client_requests[client_fd].status_code = 201;
	return ("");
}