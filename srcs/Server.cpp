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
	this->_serverConf.hostName = "localhost";
	this->_serverConf.port = "8080";
	std::cout << "Current Config" << std::endl;
	std::cout << this->_serverConf << std::endl;
	
}

Server::~Server(){
	
}


int	Server::init(){
	struct addrinfo hints;
	struct addrinfo *res;

	//Get Address info
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int	status = getaddrinfo(
		this->_serverConf.hostName.c_str(),
		this->_serverConf.port.c_str(),
		&hints,
		&res
	);
	if (status != 0){
		std::cerr << COLOR_RED << "Error: " << gai_strerror(status) << COLOR_RESET << std::endl;
		return (0);
	}
	std::cout << COLOR_GREEN << "Get address info success" << COLOR_RESET << std::endl;
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
	this->socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (this->socket_fd < 0){
		std::cerr << COLOR_RED << "Error. Unable to create socket" << strerror(errno) << std::endl;
		return (0);
	}
	std::cout << COLOR_GREEN << "Socket creation sucess" << COLOR_RESET << std::endl;

	// Allow port to be immediately reuse after close
	int	reuse = 1;
	if (setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0){
		std::cerr << COLOR_RED << "Error. Set socket option error" << strerror(errno) << std::endl;
		return (0);
	} 

	//Bind Socket
	if (bind(this->socket_fd, res->ai_addr, res->ai_addrlen) < 0){
		std::cerr << COLOR_RED << "Error. Bind failed. " << strerror(errno) << COLOR_RESET << std::endl;
		return (0);
	}
	std::cout << COLOR_GREEN << "Socket binding sucess" << COLOR_RESET << std::endl;

	if (listen(this->socket_fd, SOMAXCONN) < 0){
		std::cerr << COLOR_RED << "Error. Listen failed" << strerror(errno) << std::endl;
		return (0);
	}
	std::cout << COLOR_GREEN << "Socket listening sucess" << COLOR_RESET << std::endl;
	
	std::cout << COLOR_GREEN << "Server initialization complete" << COLOR_RESET << std::endl;
	freeaddrinfo(res);
	return (1);
}


void	Server::run(){
	std::string	client_msg;
	
	while (1){
		std::cout << "Waiting for connection" << std::endl;
		int new_socket_fd = accept(this->socket_fd, NULL, NULL);
		if (new_socket_fd < 0){
			std::cout << COLOR_RED << "Error: Accept failed" << strerror(errno) << COLOR_RESET << std::endl;
			//TODO: should continue
			return ;
		}
		client_msg = receiveAll(client_msg, new_socket_fd);
		std::cout << "Client message: " << client_msg <<std::endl;


	 std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: 13\r\n"
                               "\r\n"
                               "Hello, client!";
		send(new_socket_fd, response.c_str(), response.length(), 0);


		// int	byteRead = recv(new_socket_fd, (void*)client_msg.c_str(), BUFFER_SIZE - 1, 0);
		// while (byteRead > 0)
		// {
		// 	buffer[byteRead] = '\0';
		// 	client_msg.append(buffer);
		// 	if (parseHeader(client_msg))
		// 		break;
		// }

		// if (byteRead < 0){
		// 	//throw exception
		// 	std::cout << COLOR_RED << "Error: Receive failed" << strerror(errno) << COLOR_RESET << std::endl;
		// 	return ;
		// }
		// else{
		// 	std::cout << COLOR_GREEN << "HTTP header successfully received"  << COLOR_RESET << std::endl;
		// 	std::string  server_msg = "HTTP/1.1 200 OK\n";
		// 	server_msg += "Content-Type: text/plain\n";
		// 	server_msg += "content-Length: 30\n";

		// 	//Body
		// 	server_msg += "\nHello World!\n";
		// 	send(new_socket_fd, server_msg.c_str(), server_msg.length(), 0);
			
		// }
		
		// if (byteRead < 0){
		// 	//throw exception
		// 	std::cout << COLOR_RED << "Error: Receive failed" << strerror(errno) << COLOR_RESET << std::endl;
		// 	return ;
		// }
		// if (parseHeader(client_msg)){
		// 	std::cout << COLOR_GREEN << "HTTP header successfully received"  << COLOR_RESET << std::endl;
		// 	std::string  server_msg = "HTTP/1.1 200 OK\n";
		// 	server_msg += "Content-Type: text/plain\n";
		// 	server_msg += "content-Length: 30\n";

		// 	//Body
		// 	server_msg += "\nHello World!\n";
		// 	send(new_socket_fd, server_msg.c_str(), server_msg.length(), 0);
			
		// }
		// else{
		// 	while (byteRead > 0)
		// 	{
		// 		buffer[BUFFER_SIZE] = '\0';
		// 		client_msg.append(buffer);
		// 		byteRead = recv(new_socket_fd, buffer, BUFFER_SIZE, 0);
		// 	}
			
		// }


		// std::string  msg= "hello from webserv";
		// int bytes_send = send(new_socket_fd, msg.c_str(), msg.size(), 0);
		// std::cout << bytes_send << " bytes send" << std::endl;
	}
}

std::string		Server::receiveAll(std::string &msg, int socket_fd){
	int		bytesRead;
	char	buffer[BUFFER_SIZE];

	while (1)
	{
		memset(buffer, 0, BUFFER_SIZE);
		bytesRead = recv(socket_fd, buffer, BUFFER_SIZE, 0);
		if (bytesRead < 0){
			//throw exception;
			std::cout << "Receive error" << std::endl;
			return (msg);
		}
		msg.append(buffer);
		if (parseHeader(msg))
			break;
	}
	return (msg);
}

/**
 * @brief Check whether header is receive fully
 * 
 */
int	Server::parseHeader(std::string &msg){
	std::string	headerEnd = "\r\n\r\n";
	if (msg.find(headerEnd) == std::string::npos)
		return (0);
	return (1);
}

std::ostream&	operator<<(std::ostream& os, const serverConf& obj){
	os << "Host name: "  << obj.hostName << std::endl;
	os << "Port number: " << obj.port <<std::endl;
	os << "End of Config File";
	return (os);
}
