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
	this->conf.host = "localhost";
	this->conf.port_number.push_back("8080");
	this->conf.port_number.push_back("8081");
	this->conf.total_port = 2;

	locationInfo base;
	base.root = "/www";
	base.index = "index.html";
	base.autoindex = false;
	base.limit_except.push_back("GET");


	this->conf.locations["/"] = base;
	std::cout << "Current Config" << std::endl;
	std::cout << this->conf << std::endl;
}

Server::~Server(){
	
}


int	Server::init(){
	struct addrinfo hints;
	struct addrinfo *res;

	if (DEBUG)
		std::cout << std::endl << "Initializing server" << std::endl << std::endl;

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
			return (0);
		}
		std::cout << COLOR_GREEN << "Socket creation sucess for port " << this->conf.port_number[i] << COLOR_RESET << std::endl;
		// Allow port to be immediately reuse after close
		int	reuse = 1;
		if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0){
			std::cerr << COLOR_RED << "Error. Set socket option error" << strerror(errno) << std::endl;
			return (0);
		} 

		//Bind Socket
		if (bind(socket_fd, res->ai_addr, res->ai_addrlen) < 0){
			std::cerr << COLOR_RED << "Error. Bind failed. " << strerror(errno) << COLOR_RESET << std::endl;
			return (0);
		}
		std::cout << COLOR_GREEN << "Socket binding sucess" << COLOR_RESET << std::endl;
		if (listen(socket_fd, SOMAXCONN) < 0){
			std::cerr << COLOR_RED << "Error. Listen failed" << strerror(errno) << std::endl;
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
				if (DEBUG)
					std::cout << "Client Request " << std::endl << this->client_requests[i] << std::endl;
			}
		}
		//Write
		for (int i = 0; i < FD_SETSIZE; i++){
			if (!FD_ISSET(i, &write_ready_fd))
				continue ;
			handleRequest(i);
			sendResponse(i);
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
		if (checkReceive(socket_fd,this->client_requests[socket_fd]) == 1)
			return ;
	}
}

void	Server::sendResponse(int socket_fd){
	std::string response = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "Content-Length: 13\r\n"
                            "\r\n"
                            "Hello, client!";
	send(socket_fd, response.c_str(), response.length(), 0); 
	this->client_requests.erase(socket_fd);
	close(socket_fd);
}

int	Server::checkReceive(int socket_fd, std::string &msg){
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
		return (this->client_requests[socket_fd].length() == length);
	}
	return (1);
}

void	Server::handleRequest(int socket_fd){
	requestData request;
	
	request.head = this->client_requests[socket_fd].substr(0, this->client_requests[socket_fd].find("\r\n\r\n"));
	request.body = this->client_requests[socket_fd].substr(this->client_requests[socket_fd].find("\r\n\r\n") + 4);
	request.contentLength = this->client_requests[socket_fd].length();

	int pos = request.head.find(' ');
	std::string method = request.head.substr(0, pos);
	pos ++;
	std::string path = request.head.substr(pos, request.head.find(' ', pos) - pos);
	// if (this->conf.locations.find(path) == this->conf.locations.end()){
	// 	std::cout << "Error. Path not found" << std::endl;
	// 	return ;
	// }

	std::cout << "Request Head" << std::endl << request.head << std::endl;
	std::cout << "Request Body" << std::endl << request.body << std::endl;
	std::cout << "method: " << method << std::endl;
	std::cout << "path: " << path << std::endl;
	std::cout << this->conf.locations[path].root << std::endl;
	std::cout << this->conf.locations[path].index << std::endl;
}

std::ostream&	operator<<(std::ostream& os, const serverConf& obj){
	os << "Host name: "  << obj.host << std::endl;
	for (int i = 0; i < obj.total_port; i++){
		os << "Port number: " << obj.port_number[i] <<std::endl;
	}
	os << "End of Config File";
	return (os);
}
