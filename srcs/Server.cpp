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

Server::Server(){
	//Configure port
	this->conf.host = "127.0.0.1";
	this->conf.port_number.push_back("8080");
	this->conf.port_number.push_back("8081");
	this->conf.total_port = 2;

	//Add html
	locationInfo base;
	base.root = "./www";
	base.index = "index.html";
	base.autoindex = false;
	base.limit_except.push_back("GET");
	this->conf.locations["/"] = base;
	

	locationInfo test;
	test.root = "./www";
	test.index = "post.html";
	test.autoindex = false;
	test.limit_except.push_back("GET");
	test.limit_except.push_back("POST");
	this->conf.locations["/post"] = test;

	//Add error page
	this->conf.error_pages[404] = "./www/error/404.html";
	this->conf.error_pages[405] = "./www/error/405.html";

	//Add reason phrase
	this->reason_phrases[200] = "OK";
	this->reason_phrases[404] = "Not Found";
	this->reason_phrases[405] = "Method Not Allowed";


	std::cout << "Current Config" << std::endl;
	std::cout << this->conf << std::endl;
}

Server::~Server(){
	this->database.close();
}


int	Server::init(){
	struct addrinfo hints;
	struct addrinfo *res;

	if (DEBUG)
		std::cout << std::endl << "Initializing server" << std::endl << std::endl;
	
	//Open or Create database
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

	for (int i = 0; i < this->conf.total_port; i++){
		int	status = getaddrinfo(
			this->conf.host.c_str(),
			this->conf.port_number[i].c_str(),
			&hints,
			&res
		);
		if (status != 0){
			std::cerr << COLOR_RED << "Error: " << gai_strerror(status) << COLOR_RESET << std::endl;
			freeaddrinfo(res);
			return (0);
		}	
		std::cout << COLOR_GREEN << "Get address info success for port " << this->conf.port_number[i] << COLOR_RESET << std::endl;
		if (res->ai_next != NULL){
			std::cout << COLOR_YELLOW << "Warning: Multiple address detected. The program will use the first one by default" << COLOR_RESET << std::endl;
		}
		if (DEBUG){
			//TODO : Remove for submission as ntop is not allow
			char	ip[INET_ADDRSTRLEN];
			struct sockaddr_in	*ipv4 = reinterpret_cast<struct sockaddr_in *>(res->ai_addr);
			void 	*addr = &(ipv4->sin_addr);
			inet_ntop(res->ai_family, addr, ip, sizeof(ip));
			std::cout << "IP: " << ip << std::endl;
		}
		//Create Socket
		int socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (socket_fd < 0){
			std::cerr << COLOR_RED << "Error. Unable to create socket for port " << this->conf.port_number[i] << strerror(errno) << std::endl;
			freeaddrinfo(res);
			return (0);
		}
		std::cout << COLOR_GREEN << "Socket creation sucess for port " << this->conf.port_number[i] << COLOR_RESET << std::endl;
		// Allow port to be immediately reuse after close
		int	reuse = 1;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0){
			std::cerr << COLOR_RED << "Error. Set socket option error" << strerror(errno) << std::endl;
			freeaddrinfo(res);
			return (0);
		} 

		//Bind Socket
		if (bind(socket_fd, res->ai_addr, res->ai_addrlen) < 0){
			std::cerr << COLOR_RED << "Error. Bind failed. " << strerror(errno) << COLOR_RESET << std::endl;
			freeaddrinfo(res);
			return (0);
		}
		std::cout << COLOR_GREEN << "Socket binding sucess" << COLOR_RESET << std::endl;
		if (listen(socket_fd, SOMAXCONN) < 0){
			std::cerr << COLOR_RED << "Error. Listen failed" << strerror(errno) << std::endl;
			freeaddrinfo(res);
			return (0);
		}
		this->socket_fds.push_back(socket_fd);
		std::cout << COLOR_GREEN << "Socket listening sucess" << COLOR_RESET << std::endl;

	}
	std::cout << COLOR_GREEN << "Server initialization complete" << COLOR_RESET << std::endl;
	freeaddrinfo(res);

	if (DEBUG){
		std::cout << "All socket fd" << std::endl;
		for (size_t i = 0; i < this->socket_fds.size(); i++)
			std::cout << this->socket_fds[i] << ", ";
		std::cout << std::endl;
	}
	
	return (1);
}


void	Server::run(){
	if (DEBUG)
		std::cout << std::endl << "Server is running" << std::endl << std::endl;
	fd_set				read_ready_fd, write_ready_fd;
	struct timeval				timeout;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	FD_ZERO(&this->read_fd);
	FD_ZERO(&this->write_fd);
	for (size_t i = 0;i < this->socket_fds.size(); i++){
		FD_SET(this->socket_fds[i], &this->read_fd);
	}
	while (1){
		memcpy(&read_ready_fd, &this->read_fd, sizeof(fd_set));
		memcpy(&write_ready_fd, &this->write_fd, sizeof(fd_set));

		int select_val = select(FD_SETSIZE, &read_ready_fd, &write_ready_fd, NULL, &timeout);
		if (select_val < 0){
			std::cout << COLOR_RED << "Error. select failed. " << strerror(errno) << COLOR_RESET << std::endl;
			return ;
		}
		for (int i = 0; i < FD_SETSIZE; i++){
			if (!FD_ISSET(i, &read_ready_fd))
				continue ;
			int	is_new_socket = 0;
			for (size_t fd = 0; fd < this->socket_fds.size(); fd++){
				if (i == this->socket_fds[fd]){
					int new_socket = acceptNewConnection(this->socket_fds[fd]);
					if (DEBUG)
						std::cout << new_socket << "new client joined at " << this->socket_fds[fd] << std::endl;
					if (new_socket < 0)
						continue;
					FD_SET(new_socket, &this->read_fd);
					is_new_socket = 1;
					break ;
				}
			}
			if (!is_new_socket){
				handleConnection(i);
				FD_SET(i, &this->write_fd);
        		FD_CLR(i, &this->read_fd);
			}
		}
		//Write
		for (int i = 0; i < FD_SETSIZE; i++){
			if (!FD_ISSET(i, &write_ready_fd))
				continue ;
			int res = handleRequest(i);
			sendResponse(i, res);
			FD_CLR(i, &this->write_fd);
		}
	}
}


int	Server::acceptNewConnection(int socket_fd){
	int	new_socket = accept(socket_fd, NULL, NULL);
	if (new_socket < 0){
		std::cout << COLOR_RED << "Error: Accept failed " << strerror(errno) << COLOR_RESET << std::endl;
		close(socket_fd);
		return (-1);
	}
	fcntl(new_socket, F_SETFL, O_NONBLOCK);
	return (new_socket);
}

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
			std::cout << "Error. recv failed at " << socket_fd << std::endl;
			this->client_requests.erase(socket_fd);
			close(socket_fd);
			return ;
		}
		this->client_requests[socket_fd].append(buffer, bytes_read);
		if (checkReceive(this->client_requests[socket_fd]) == 1)
			return ;
	}
}

/**
 * @brief Evaluate the request header and body.
 * Write the response body to client_responses 
 */
int		Server::handleRequest(int socket_fd){
	requestData request;
	
	request.header = this->client_requests[socket_fd].substr(0, this->client_requests[socket_fd].find("\r\n\r\n"));
	request.body = this->client_requests[socket_fd].substr(this->client_requests[socket_fd].find("\r\n\r\n") + 4);
	request.contentLength = this->client_requests[socket_fd].length();

	int pos = request.header.find(' ');
	std::string method = request.header.substr(0, pos);
	pos ++;
	std::string path = request.header.substr(pos, request.header.find(' ', pos) - pos);
	if (DEBUG){
		std::cout << "Request Header: " << std::endl << request.header << std::endl;
		std::cout << "Request Body: " << std::endl << request.body << std::endl;
		std::cout << "Method: " << method << std::endl;
		std::cout << "path: " << path << std::endl;
		std::cout << this->conf.locations[path].root << std::endl;
		std::cout << this->conf.locations[path].index << std::endl;
	}
	//Check if request can be handled
	if (this->conf.locations.find(path) == this->conf.locations.end()){
		std::cout << COLOR_RED <<  "Error. Path: " << path <<  " not found"  << COLOR_RESET << std::endl;
		this->client_responses[socket_fd] = this->handleError(404);
		return (404);
	}
	int request_method = checkMethod(method, this->conf.locations[path].limit_except);
	if (request_method == -1){
		std::cout << COLOR_RED << "Error. Method " << method << " is not allowed"  << COLOR_RESET << std::endl;
		this->client_responses[socket_fd] = this->handleError(405);
		return (405);
	}

	//TODO: handle when no index but have autoindex on
	std::ifstream file(this->conf.locations[path].root + "/" + this->conf.locations[path].index);
	if (!file.is_open()){
		std::cout << COLOR_RED << "Error. File not found" << COLOR_RESET << std::endl;
		this->client_responses[socket_fd] = this->handleError(404);
	}
	
	typedef std::string (Server::*func)(requestData &,std::ifstream &);
	METHOD method_enum = static_cast<METHOD>(request_method);
	func methods[METHOD_COUNT] = {&Server::handleGet, &Server::handlePost, &Server::handlePut, &Server::handleHead, &Server::handleDelete};
	this->client_responses[socket_fd] = (this->*methods[method_enum])(request, file);
	// std::string body = (this->*methods[method_enum])(file);
	//get body
	//add content length
	return (200);
}

/**
 * @brief Add header to the response body and sent it
 */
void	Server::sendResponse(int socket_fd, int status_code){
	std::string res = "HTTP/1.1 ";
	res += std::to_string(status_code);
	res += " ";
	res += this->reason_phrases[status_code];
	res += "\r\n";
	res += "Content-Type: text/html\r\n";
	res += "Content-Length: ";
	res += std::to_string(this->client_responses[socket_fd].length());
	res += "\r\n\r\n";
	res += this->client_responses[socket_fd];
	
	int byteSend = send(socket_fd, res.c_str(), res.length(), 0);
	if (byteSend < 0)
		std::cout << COLOR_RED << "Error. Send failed at " << socket_fd << strerror(errno) << COLOR_RESET << std::endl;
	this->client_requests.erase(socket_fd);
	this->client_responses.erase(socket_fd);
	close(socket_fd);
}

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

std::string Server::handleError(int status_code){
	std::ifstream input;
	std::stringstream buffer;

	std::string line;
	if (status_code == 404){
		input.open(this->conf.error_pages[404]);
		buffer << input.rdbuf();
		return (buffer.str());
	}
	else if (status_code == 405){
		input.open(this->conf.error_pages[405]);
		buffer << input.rdbuf();
		return (buffer.str());
	}
	return ("");
}

/**
 * @brief Read the file path of a target location and return the body of response
 */
//Input parameter is request data and location info struct
std::string	Server::handleGet(requestData &request, std::ifstream &file){
	(void)request;
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();
	return (content);
}

std::string	Server::handlePost(requestData &request, std::ifstream &file){
	(void)file;
	// std::string content = request.header.find
	std::string line;
	std::istringstream iss(request.header);
	while (std::getline(iss, line)) {
        if (line.find("Content-Type") != std::string::npos) {
            std::cout << line << std::endl;
        }
    }
	// std::stringstream buffer;
	// buffer << file.rdbuf();
	// std::string content = buffer.str();

	// std::string res = "HTTP/1.1 200 OK\r\n"
	// 						"Content-Type: text/html\r\n"
	// 						"Content-Length: " + std::to_string(content.length()) + "\r\n"
	// 						"\r\n" + content;
	return ("");
}

std::string	Server::handlePut(requestData &request, std::ifstream &file){
	(void)request;
	(void)file;
	// std::stringstream buffer;
	// buffer << file.rdbuf();
	// std::string content = buffer.str();

	// std::string res = "HTTP/1.1 200 OK\r\n"
	// 						"Content-Type: text/html\r\n"
	// 						"Content-Length: " + std::to_string(content.length()) + "\r\n"
	// 						"\r\n" + content;
	return ("");
}

std::string	Server::handleHead(requestData &request, std::ifstream &file){
	(void)request;
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();

	std::string res = "HTTP/1.1 200 OK\r\n"
							"Content-Type: text/html\r\n"
							"Content-Length: " + std::to_string(content.length()) + "\r\n"
							"\r\n" + content;
	return (res);
}

std::string	Server::handleDelete(requestData &request, std::ifstream &file){
	(void)request;
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();

	std::string res = "HTTP/1.1 200 OK\r\n"
							"Content-Type: text/html\r\n"
							"Content-Length: " + std::to_string(content.length()) + "\r\n"
							"\r\n" + content;
	return (res);
}

std::ostream&	operator<<(std::ostream& os, const serverConf& obj){
	os << "Host name: "  << obj.host << std::endl;
	for (int i = 0; i < obj.total_port; i++){
		os << "Port number: " << obj.port_number[i] <<std::endl;
	}
	os << "End of Config File";
	return (os);
}
