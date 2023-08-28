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

			//Allow Port Reuse
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
 * @brief Server run function. Will run a infinite loop to accept connection and handle request
 */
void	Server::run(){
	std::cout << "Server is running" << std::endl;
	
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
			return ;
		}

		//Reading Request
		for (int i = 0; i < FD_SETSIZE; i++){
			if (!FD_ISSET(i, &read_ready_fd))
				continue ;
			int is_new_socket = 0;
			if (this->servers.find(i) != this->servers.end()){
				int new_socket = acceptNewConnection(i);
				if (new_socket < 0)
					continue ;
				FD_SET(new_socket, &this->read_fd);
				is_new_socket = 1;
				break ;
			}
			if (!is_new_socket){
				handleConnection(i);
				FD_SET(i, &this->write_fd);
				FD_CLR(i, &this->read_fd);
			}
		}

		//Writing Response
		for(int i = 0; i < FD_SETSIZE; i ++){
			if (!FD_ISSET(i, &write_ready_fd))
				continue ;
			handleRequest(i);
			sendResponse(i);
			FD_CLR(i, &this->write_fd);
		}
	}
}

/**
 * @brief Accept new connection and map the client to correct server
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
	if (DEBUG){
		std::cout << "New client joined at socket " << new_socket << std::endl;
		std::cout << "Join at port " << this->sockets_port[socket_fd] << std::endl;
	}
	fcntl(new_socket, F_SETFL, O_NONBLOCK);
	return (new_socket);
}

/**
 * @brief Receive the message sent by client
 */
void	Server::handleConnection(int socket_fd){
	int		bytes_read;
	char	buffer[BUFFER_SIZE];

	while (1){
		memset(buffer, 0, BUFFER_SIZE);
		bytes_read = recv(socket_fd, buffer, BUFFER_SIZE, 0);
		if (bytes_read == 0){
			std::cout << "Client hangout at " << socket_fd << std::endl;
			this->client_requests.erase(socket_fd);
			close(socket_fd);
			return ;
		}
		if (bytes_read < 0){
			std::cout << "Error. recv failed at " << socket_fd << ". " << strerror(errno) <<  std::endl;
			this->client_responses[socket_fd] = this->handleError(500, this->client_requests[socket_fd].server_fd);
			this->client_requests[socket_fd].status_code = 500;
			sendResponse(socket_fd);
			return ;
		}
		this->client_requests[socket_fd].whole_request.append(buffer, bytes_read);
		if (checkReceive(this->client_requests[socket_fd].whole_request) == 1)
			return ;
	}
}

/**
 * @brief Evaluate the request header and body.
 * Write the response body to client_responses
 */
void		Server::handleRequest(int socket_fd){
	std::string header = this->client_requests[socket_fd].whole_request.substr(0, this->client_requests[socket_fd].whole_request.find("\r\n\r\n"));
	std::string body = this->client_requests[socket_fd].whole_request.substr(this->client_requests[socket_fd].whole_request.find("\r\n\r\n") + 4);
	size_t contentLength = body.length();
	this->client_requests[socket_fd].header = header;
	this->client_requests[socket_fd].body = body;
	this->client_requests[socket_fd].contentLength = contentLength;

	int pos = header.find(' ');
	std::string method = header.substr(0, pos);
	pos ++;
	std::string path = header.substr(pos, header.find(' ', pos) - pos);
	this->client_requests[socket_fd].path = path;

	int	server_fd = this->client_requests[socket_fd].server_fd;
	if (DEBUG){
		std::cout << "Request Header: " << std::endl << header << std::endl;
		std::cout << "Request Body: " << std::endl << body << std::endl;
		std::cout << "Method: " << method << std::endl;
		std::cout << "Path: " << path << std::endl;
		std::cout << "Path Root: " << this->servers[server_fd].locations[path]->root << std::endl;
		std::cout << "Path Index: " << this->servers[server_fd].locations[path]->index << std::endl;
	}

	if (!checkHost(this->client_requests[socket_fd].header, this->servers[server_fd].server_name)){
		std::cout << COLOR_RED << "Error. Host and server name does not match" << COLOR_RESET << std::endl;
		this->client_responses[socket_fd] = this->handleError(400, this->client_requests[socket_fd].server_fd);
		this->client_requests[socket_fd].status_code = 400;
		return ;
	}

	if (path == "/favicon.ico"){
		std::cout << COLOR_YELLOW << "Favicon requested" << COLOR_RESET << std::endl;
		this->client_responses[socket_fd] = this->handleError(404, this->client_requests[socket_fd].server_fd);
		this->client_requests[socket_fd].status_code = 404;
		return ;
	}

	//Check if the path is valide. Will perform two check
	//	First check for the path is exact match with what present in the location
	//	If no, check for prefix match
	//If not found, return 404
	if (this->servers[server_fd].locations.find(path) == this->servers[server_fd].locations.end()){
		std::string file_path = checkDirectoryRoute(server_fd, path);
		if (file_path.empty()){
			std::cout << COLOR_RED <<  "Error. Path: " << path <<  " not found"  << COLOR_RESET << std::endl;
			this->client_responses[socket_fd] = this->handleError(404, this->client_requests[socket_fd].server_fd);
			this->client_requests[socket_fd].status_code = 404;
			return ;
		}
		else
			this->client_requests[socket_fd].file_path = file_path;
	}

	//Check for the content-length exceed the limit
	if (this->servers[server_fd].locations[path]->max_body_size_set == 1){
		if (this->client_requests[socket_fd].contentLength > this->servers[server_fd].locations[path]->max_body_size){
			std::cout << COLOR_RED << "Error. Content-Length exceed limit" << COLOR_RESET << std::endl;
			this->client_responses[socket_fd] = this->handleError(413, this->client_requests[socket_fd].server_fd);
			this->client_requests[socket_fd].status_code = 413;
			return ;
		}
	}
	
	//Check if is cgi script
	if (!this->client_requests[socket_fd].file_path.empty()){
		std::string::size_type extension_pos = this->client_requests[socket_fd].file_path.find_last_of('.');
		if (extension_pos != std::string::npos){
			std::string extension = this->client_requests[socket_fd].file_path.substr(extension_pos);
			std::string script_file = this->client_requests[socket_fd].file_path.substr(1);
			serverConf conf = this->servers[this->client_requests[socket_fd].server_fd];
			std::multimap<std::string, std::string>::iterator it = conf.cgi.find(extension);
			if (it != conf.cgi.end()){
				if (method != "GET" && method != "POST"){
					std::cout << COLOR_RED << "Error. Method " << method << " is not allowed"  << COLOR_RESET << std::endl;
					this->client_responses[socket_fd] = this->handleError(405, this->client_requests[socket_fd].server_fd);
					this->client_requests[socket_fd].status_code = 405;
					return ;
				}
				// std::cout << conf.root << std::endl;
				// std::cout << conf.cgi_bin << std::endl;
				// std::cout << method << std::endl;
				if (DEBUG){
					std::cout << "extension: " << extension << std::endl;
					std::cout << "script_file: " << script_file << std::endl;
					std::cout << "Handler: " << it->second << std::endl;
				}
				this->client_responses[socket_fd] = this->handleCGI(socket_fd, conf, method);
				return ;
			}
		}
	}


	//Check if Method can be handle
	METHOD request_method = getMethod(method, this->servers[server_fd].locations[path]->limit_except);
	this->client_requests[socket_fd].method = request_method;
	int method_int = static_cast<int>(request_method);
	if (method_int < 0){
		if (method_int == -2){
			std::cout << COLOR_RED << "Error. Method " << method << " is not allowed"  << COLOR_RESET << std::endl;
			this->client_responses[socket_fd] = this->handleError(405, this->client_requests[socket_fd].server_fd);
			this->client_requests[socket_fd].status_code = 405;
			return ;
		}
		if (method_int == -1){
			std::cout << COLOR_RED << "Error. Unknown method. " << method << COLOR_RESET << std::endl;
			this->client_responses[socket_fd] = this->handleError(501, this->client_requests[socket_fd].server_fd);
			this->client_requests[socket_fd].status_code = 501;
			return ;
		}
	}
	
	this->client_requests[socket_fd].status_code = 200;
	typedef std::string (Server::*func)(int &, locationInfo &);
	func methods[METHOD_COUNT] = {&Server::handleGet, &Server::handlePost, &Server::handlePut, &Server::handleHead, &Server::handleDelete};
	this->client_responses[socket_fd] = (this->*methods[this->client_requests[socket_fd].method])
											(socket_fd, *(this->servers[server_fd].locations[path]));
}

/**
 * @brief Add header to the response body and sent it
 */
void	Server::sendResponse(int socket_fd){
	int status_code = this->client_requests[socket_fd].status_code;
	std::string res = "HTTP/1.1 ";
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
		res += this->client_responses[socket_fd];
	}
	
	int byteSend = send(socket_fd, res.c_str(), res.length(), 0);
	if (byteSend < 0)
		std::cout << COLOR_RED << "Error. Send failed at " << socket_fd << strerror(errno) << COLOR_RESET << std::endl;
	this->client_requests.erase(socket_fd);
	this->client_responses.erase(socket_fd);
	if (DEBUG)
		std::cout << "Sent to socket: " << socket_fd << std::endl;
	close(socket_fd);
}

/**
 * @brief Check that all the data has been received
 */
int	Server::checkReceive(std::string &msg){
	std::string header_end = "\r\n\r\n";
	if (msg.find(header_end) == std::string::npos)
		return (1);
	if (msg.find("Transfer-Encoding: chunked") != std::string::npos){
		if (msg.find("0\r\n\r\n") == std::string::npos)
			return (0);
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
 * @brief Read the respective error code and write the html file to the response body
 */
std::string Server::handleError(int status_code, int server_fd){
	serverConf conf = this->servers[server_fd];
	std::string res = conf.error_pages[status_code];
	std::ifstream file(res.c_str());
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();
	file.close();
	return (content);
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

std::string Server::handleCGI(int &client_fd, serverConf &server, std::string &method){
	(void) method;
	(void) server;
	std::cout << "HANDLE CGI" << std::endl;

	// std::string cgi_path = server.cgi_bin + this->client_requests[client_fd].file_path;
	std::string cgi_path = this->client_requests[client_fd].file_path;

	if (access(cgi_path.c_str(), R_OK | X_OK) != 0){
		std::cout << COLOR_RED << "Error: Unable to access cgi script: " << cgi_path << COLOR_RESET << std::endl;
		this->client_requests[client_fd].status_code = 500;
		return (this->handleError(500, this->client_requests[client_fd].server_fd));
	}

	int stdin = dup(STDIN_FILENO);
	int stdout = dup(STDOUT_FILENO);

	//tempIn is used to store the duplicate script, tempOut is used to store the output
	FILE *tempIn = std::tmpfile();
	FILE *tempOut = std::tmpfile();
	int fdIn = fileno(tempIn);
	int fdOut = fileno(tempOut);
	std::string res;
	
	//Write the script to tempIn
	FILE *script = fopen(cgi_path.c_str(), "r");
	if (script == NULL){
		std::cout << COLOR_RED << "Error: Unable to open cgi script: " << cgi_path << COLOR_RESET << std::endl;
		this->client_requests[client_fd].status_code = 500;
		return (this->handleError(500, this->client_requests[client_fd].server_fd));
	}

	char buffer[1024];
	int bytes_read;
	while ((bytes_read = std::fread(buffer, 1, 1024, script)) > 0){
		std::fwrite(buffer, 1, bytes_read, tempIn);
	}
	fclose(script);
	std::rewind(tempIn);

	pid_t child = fork();
	if (child < 0){
		std::cout << COLOR_RED << "Error: Unable to fork" << COLOR_RESET << std::endl;
		fclose(tempIn);
		fclose(tempOut);
		this->client_requests[client_fd].status_code = 500;
		return (this->handleError(500, this->client_requests[client_fd].server_fd));
	}
	if (child == 0){
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		execve(cgi_path.c_str(), NULL, NULL);
	}
	else{
		int status;
		waitpid(child, &status, 0);
		char buffer[1024];
		std::rewind(tempOut);
		while ((bytes_read = std::fread(buffer, 1, 1024, tempOut)) > 0){
			res.append(buffer, bytes_read);
		}
	}
	dup2(stdin, STDIN_FILENO);
	dup2(stdout, STDOUT_FILENO);
	fclose(tempIn);
	fclose(tempOut);

	return (res);
}



/**
 * @brief Read the file path of a target location and return the body of response
 * Handle get request, will follow order below
 * 		- Search for specific file eg. /index.html. If not found will sent 404
 * 		- Handle redirection
 * 		- Use index parameter
 * 		- Use autoindex parameter
 */
std::string	Server::handleGet(int &client_fd, locationInfo &location){
	requestData &request = this->client_requests[client_fd];
	if (request.file_path.empty() && location.index.empty() && location.redirect_address.empty()){
		if (location.autoindex == true){

			return (generateAutoindex(client_fd, request.path, location.root));
		}
		else{
			this->client_requests[client_fd].status_code = 404;
			return (handleError(404, this->client_requests[client_fd].server_fd));
		}
	}
	
	std::string whole_path;
	if (!request.file_path.empty()){
		whole_path = location.root + request.file_path;
		if (checkIsDirectory(whole_path) == 1){
			std::map<std::string, locationInfo *>::iterator it;
			it = this->servers[request.server_fd].locations.find(request.path);
			if (it != this->servers[request.server_fd].locations.end()){
				if (it->second->autoindex == true){
					return (generateAutoindex(client_fd, request.path, whole_path));
				}
			}
			this->client_requests[client_fd].status_code = 404;
			return (handleError(404, this->client_requests[client_fd].server_fd));
		}
	}
	else if (!location.redirect_address.empty()){
		this->client_requests[client_fd].status_code = 301;
		return (location.redirect_address);
	}
	else{
		whole_path = location.root + "/" + location.index;
	}
	std::ifstream file(whole_path.c_str());
	if (!file.is_open()){
		std::cout << COLOR_RED << "Error. File not found. " << whole_path << COLOR_RESET << std::endl;
		this->client_requests[client_fd].status_code = 404;
		return (handleError(404, this->client_requests[client_fd].server_fd));
	}
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();
	file.close();
	return (content);
}

/**
 * @brief The server will only handle two type of content type: "text/plain" and "multipart/form-data", other will return 400.
 * "text/plain" will stored the request body in database.txt
 * "multipart/form-data" will save the file in the upload directory
 */
std::string	Server::handlePost(int &client_fd, locationInfo &location){
	requestData request = this->client_requests[client_fd];
	std::string::size_type content_type = request.header.find("Content-Type: ");

	if (content_type == std::string::npos){
		this->client_requests[client_fd].status_code = 400;
		return (this->handleError(400, this->client_requests[client_fd].server_fd));
	}

	if (location.max_body_size_set == 1){
		std::string content = this->client_requests[client_fd].body;
		if (content.size() > location.max_body_size){
			this->client_requests[client_fd].status_code = 413;
			return (this->handleError(413, this->client_requests[client_fd].server_fd));
		}
	}
	
	std::string content_type_value = request.header.substr(content_type + 14, request.header.find("\r\n", content_type) - content_type - 14);
	
	if (content_type_value.find("multipart/form-data") == std::string::npos &&
		 content_type_value != "text/plain"){
	}

	if (content_type_value == "text/plain"){
		std::cout << "Handle text plain" << std::endl;
		return (handlePostText(client_fd));
	}

	else if (content_type_value.find("multipart/form-data") != std::string::npos){
		std::cout << "Handle upload" << std::endl;
		return (handleUpload(client_fd, location, 0));
	}
	else{
		this->client_requests[client_fd].status_code = 415;
		return (this->handleError(415, this->client_requests[client_fd].server_fd));
	}
}

/**
 * @brief A PUT request is handled the almost the same way as POST request. 
 * The only difference is that PUT will overwrite the file if it exist and return 200 when overwrite happen
 */
std::string	Server::handlePut(int &client_fd, locationInfo &location){
	requestData request = this->client_requests[client_fd];
	std::string::size_type content_type = request.header.find("Content-Type: ");

	if (content_type == std::string::npos){
		this->client_requests[client_fd].status_code = 400;
		return (this->handleError(400, this->client_requests[client_fd].server_fd));
	}

	if (location.max_body_size_set == 1){
		std::string content = this->client_requests[client_fd].body;
		if (content.size() > location.max_body_size){
			this->client_requests[client_fd].status_code = 413;
			return (this->handleError(413, this->client_requests[client_fd].server_fd));
		}
	}
	
	std::string content_type_value = request.header.substr(content_type + 14, request.header.find("\r\n", content_type) - content_type - 14);
	
	if (content_type_value.find("multipart/form-data") == std::string::npos &&
		 content_type_value != "text/plain"){
	}

	if (content_type_value == "text/plain"){
		std::string content = this->client_requests[client_fd].body;
		std::ifstream file("database.txt");
		std::string line;
		std::string whole_line;
		while (std::getline(file, content)){
			whole_line += line;
		}
		if (whole_line.find(content) != std::string::npos){
			std::cout << "Repeat Request" << std::endl;
			this->client_requests[client_fd].status_code = 200;
			return ("");
		}
		else{
			std::cout << "Not Repeated Request" << std::endl;
			std::cout << "Handle Text Plain" << std::endl;
			return (handlePostText(client_fd));
		}
	}

	else if (content_type_value.find("multipart/form-data") != std::string::npos){
		return (handleUpload(client_fd, location, 1));
	}
	else{
		this->client_requests[client_fd].status_code = 415;
		return (this->handleError(415, this->client_requests[client_fd].server_fd));
	}
}

std::string	Server::handleHead(int &client_fd, locationInfo &location){
	std::cout << "Handle Head" << std::endl;

	std::string whole_path;
	if (!this->client_requests[client_fd].file_path.empty()){
		whole_path = location.root + this->client_requests[client_fd].file_path;
	}
	else{
		whole_path = location.root + "/" + location.index;
	}
	std::ifstream file(whole_path.c_str());
	if (!file.is_open()){
		std::cout << COLOR_RED << "Error. File not found. " << whole_path << COLOR_RESET << std::endl;
		this->client_requests[client_fd].status_code = 404;
		return ("");
	}
	this->client_requests[client_fd].status_code = 200;
	file.close();
	return ("");
}

std::string	Server::handleDelete(int &client_fd, locationInfo &location){
	if (this->client_requests[client_fd].file_path.empty()){
		this->client_requests[client_fd].status_code = 404;
		return (handleError(404, this->client_requests[client_fd].server_fd));
	}
	std::string whole_path = location.root + this->client_requests[client_fd].file_path;
	if (std::remove(whole_path.c_str()) != 0){
		this->client_requests[client_fd].status_code = 404;
		return (handleError(404, this->client_requests[client_fd].server_fd));
	}
	this->client_requests[client_fd].status_code = 204;
	return ("");
}

/**
 * @brief Called when no exact match found in route. Will check for prefix match and return the file path. 
 * Also modify the input argument to remove the file name from the path.
 * Return empty string if not found.
 */
std::string Server::checkDirectoryRoute(int server_fd, std::string &path){
	std::string file_path;

	std::map<std::string, locationInfo *>::iterator it;
	for (it = this->servers[server_fd].locations.begin(); it != this->servers[server_fd].locations.end(); it ++){
		const std::string &base_path = it->first;
		if (path.substr(0, base_path.length()) == base_path){
			file_path = path.substr(base_path.length());
		}
	}
	path = path.substr(0, path.length() - file_path.length());
	if (file_path[0] != '/'){
		file_path = "/" + file_path;
	}
	return (file_path);
}

std::string	Server::generateAutoindex(int &client_fd, std::string &route, std::string &file_path){
		std::cout << "Route: " << route << std::endl;
		std::cout << "File Path: " << file_path << std::endl;

		if (checkIsDirectory(file_path) <= 0){
			this->client_requests[client_fd].status_code = 403;
			return (handleError(403, this->client_requests[client_fd].server_fd));
		}
		std::string res;
		res += "<html>\n<head>\n<title>Index of " + route + "</title>\n</head>\n<body>\n<h1>Index of " + route + "</h1>\n";
		res += "<table>\n<tr>\n<th>Name</th>\n<th>Last Modified</th>\n<th>Size</th>\n</tr>\n";
		try{
			res += printDirectory(route, file_path);
		}
		catch(const std::exception &e)
		{
			std::cout << COLOR_RED << "Error. Unable to open directory " << file_path << COLOR_RESET << std::endl;
			this->client_requests[client_fd].status_code = 500;
			return (handleError(500, this->client_requests[client_fd].server_fd));
		}
		res += "</table></body>\n</html>";
		return (res);
}

/**
 * @brief Method is used to determine whether to create the file or overwrite the file. 0 for post, 1 for put
 */
std::string	Server::handleUpload(int &client_fd, locationInfo &location, int method){

	std::string upload_dir = location.root + location.upload_path;
	if (DEBUG)
		std::cout << "Upload Directory: " << upload_dir << std::endl;
	if (checkIsDirectory(upload_dir) != 1){
		std::cout << COLOR_RED << "Error. Upload directory not found" << COLOR_RESET << std::endl;
		this->client_requests[client_fd].status_code = 500;
		return (this->handleError(500, this->client_requests[client_fd].server_fd));
	}

	DIR *dir = opendir(upload_dir.c_str());
	if (dir == NULL){
		std::cout << COLOR_RED << "Error. Unable to open upload directory" << COLOR_RESET << std::endl;
		this->client_requests[client_fd].status_code = 500;
		return (this->handleError(500, this->client_requests[client_fd].server_fd));
	}
	std::string::size_type start_pos = this->client_requests[client_fd].header.find("boundary=");
	std::string boundary_value = "--" + this->client_requests[client_fd].header.substr(start_pos + 9, 
		this->client_requests[client_fd].header.find("\r\n", start_pos) - start_pos - 9);
	
	formData form_data = parseUpload(this->client_requests[client_fd].body, boundary_value);
	if (method == 1){
		struct dirent *ent;
		while ((ent = readdir(dir)) != NULL) {
        	if (std::strcmp(ent->d_name, form_data.filename.c_str()) == 0) {
            	std::cout << "Repeated file: " << form_data.filename << std::endl;
				this->client_requests[client_fd].status_code = 200;
				return ("");
			}
		}
	}
	if (storeFile(upload_dir, form_data) == 0){
		closedir(dir);
		this->client_requests[client_fd].status_code = 500;
		return (this->handleError(500, this->client_requests[client_fd].server_fd));
	}
	this->client_requests[client_fd].status_code = 201;
	closedir(dir);
	return ("");
}

std::string Server::handlePostText(int &client_fd){
	std::string content = this->client_requests[client_fd].body;
	this->database << content << std::endl;
	this->database.flush();
	this->client_requests[client_fd].status_code = 201;
	return ("");
}