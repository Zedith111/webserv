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
	this->_serverConf.portNumber.push_back(8080);
	this->_serverConf.portNumber.push_back(8081);
	this->_serverConf.totalPort = 2;
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

	for(int i = 0; i < this->_serverConf.totalPort; i ++){
		int	status = getaddrinfo(
			this->_serverConf.hostName.c_str(),
			std::to_string(this->_serverConf.portNumber[i]).c_str(),
			&hints,
			&res
		);
		if (status != 0){
			std::cerr << COLOR_RED << "Error: " << gai_strerror(status) << COLOR_RESET << std::endl;
			return (0);
		}	
		std::cout << COLOR_GREEN << "Get address info success for port " << this->_serverConf.portNumber[i] << COLOR_RESET << std::endl;
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
			std::cerr << COLOR_RED << "Error. Unable to create socket for port " << this->_serverConf.portNumber[i] << strerror(errno) << std::endl;
			return (0);
		}
		std::cout << COLOR_GREEN << "Socket creation sucess for port " << this->_serverConf.portNumber[i] << COLOR_RESET << std::endl;
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
		for(size_t i = 0; i < this->socket_fds.size(); i++){
			std::cout << this->socket_fds[i] << std::endl;
	}
	}
	
	return (1);
}


// void	Server::run(){
// 	std::string	client_msg;
// 	//TODO: put this is class member variable
// 	fd_set		read_fds;
// 	fd_set		read_ready_fd;

// 	FD_ZERO(&read_fds);
// 	FD_ZERO(&read_ready_fd);
// 	for (int i = 0; i < this->_serverConf.totalPort; i ++){
// 		FD_SET(this->socketFds[i], &read_fds);
// 	}
// 	while (1){
// 		std::cout << "Waiting for connection" << std::endl;

// 		memcpy(&read_ready_fd, &read_fds, sizeof(fd_set));
		
// 		if (select(FD_SETSIZE, &read_ready_fd, NULL, NULL, NULL) < 0){
// 			std::cout << COLOR_RED << "Error: Select failed " << strerror(errno) << COLOR_RESET << std::endl;
// 			return ;
// 		}
// 		//if select == 0

// 		//Check through all fd_set
// 		for (int i = 0; i < FD_SETSIZE; i++){
// 			if (FD_ISSET(i, &read_ready_fd)){
// 				for (size_t so = 0; so < this->socketFds.size(); so++){
// 					//If the fd equal the original socket fd, means is a new connection
// 					if (i == this->socketFds[so]){
// 						int	client_socket = accept(this->socketFds[so], NULL, NULL);
// 						if (client_socket < 0){
// 							std::cout << COLOR_RED << "Error: Accept failed " << strerror(errno) << COLOR_RESET << std::endl;
// 							//TODO: should continue
// 							return ;
// 						}
// 						FD_SET(client_socket, &read_fds);
// 					}
// 					else{
// 						// client_msg = receiveRequest(client_msg, i);
// 	 					// std::string response = "HTTP/1.1 200 OK\r\n"
//                         //     	"Content-Type: text/html\r\n"
//                         //     	"Content-Length: 13\r\n"
//                         //        "\r\n"
//                         //        "Hello, client!";
// 						// 
// 					}
// 				}
// 			}
// 		}
// 	}
// }

void	Server::run(){
	fd_set				read_ready_fd, write_ready_fd;
	timeval				timeout;

	FD_ZERO(&this->read_fd);
	FD_ZERO(&this->write_fd);
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	for(size_t i = 0;i < this->socket_fds.size(); i++){
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
		for (int i = 0; i < FD_SETSIZE; i ++){
			if (!FD_ISSET(i, &read_ready_fd))
				continue ;
			int	is_new_socket = 0;
			for (size_t fd = 0; fd < this->socket_fds.size(); fd++){
				if (i == this->socket_fds[fd]){
					int new_socket = acceptNewConnection(this->socket_fds[fd]);
					if (DEBUG){
						std::cout << new_socket << "joined at " << this->socket_fds[fd] << std::endl;
					}
					if (new_socket < 0)
						continue;
					FD_SET(new_socket, &this->read_fd);
					is_new_socket = 1;
					break ;
				}
			}
			if (!is_new_socket){
				handleConnection(i);
        		FD_CLR(i, &this->read_fd);
			}
		}
	}
}


int	Server::acceptNewConnection(int socket_fd){
	int	new_socket = accept(socket_fd, NULL, NULL);
	if (new_socket < 0){
		std::cout << COLOR_RED << "Error: Accept failed " << strerror(errno) << COLOR_RESET << std::endl;
		return (-1);
	}
	return (new_socket);
}

void	Server::handleConnection(int socket_fd){
	std::string client_msg = receiveRequest(socket_fd);
	
	// if (DEBUG)
	// 	std::cout << client_msg << std::endl;
	std::string response = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "Content-Length: 13\r\n"
                            "\r\n"
                            "Hello, client!";
	send(socket_fd, response.c_str(), response.length(), 0); 
	// close(socket_fd);		
}

std::string		Server::receiveRequest(int socket_fd){
	std::string msg;
	int		bytesRead;
	char	buffer[BUFFER_SIZE];
	while (1)
	{
		memset(buffer, 0, BUFFER_SIZE);
		bytesRead = recv(socket_fd, buffer, BUFFER_SIZE, 0);
		if (bytesRead == 0){
			std::cout << "Client hang out" << std::endl;
		}
		if (bytesRead < 0){
			//TODO:throw exception;
			std::cout << "Receive error" << std::endl;
			return (msg);
		}
		msg.append(buffer, bytesRead);
		if (parseHttpHeader(msg))
			break;
	}
	return (msg);
}

std::ostream&	operator<<(std::ostream& os, const serverConf& obj){
	os << "Host name: "  << obj.hostName << std::endl;
	os << "Port number: " << obj.port <<std::endl;
	os << "End of Config File";
	return (os);
}
