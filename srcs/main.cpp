/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/26 16:42:49 by zah               #+#    #+#             */
/*   Updated: 2023/07/06 21:13:39 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

int	main(int argc, char **argv){
	std::string	filePath = "config/default.conf";
	if (argc > 2){
		std::cout << COLOR_RED << " Error. Usage : webserv <Configuration File>"  << COLOR_RESET << std::endl;
		return (1);
	}
	if (argc < 2)
		std::cout << COLOR_YELLOW << "Warning. No configuration file provided. A default file will be used" << COLOR_RESET << std::endl;
	else{
		filePath = argv[1];
	}
	ConfigParser confParser;
	if (!confParser.parse(filePath))
		return (1);
	Server server;
	if (!server.init())
		return (1);
	server.run();

	// int	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	// (void) argc;
	// (void) argv;
	// //Will pass through configuration file
	// struct sockaddr_in	address;
	// const int			port = 8081;
	// const int			backlog = 5;
	// int					addrlen = sizeof(address);


	// if (server_fd < 0){
	// 	std::cerr << "Error. Unable to create socket" << strerror(errno) << std::endl;
	// 	return (1);
	// }
	// memset((char *)&address, 0, sizeof(address));
	// address.sin_family = AF_INET;
	// address.sin_addr.s_addr = htonl(INADDR_ANY);
	// address.sin_port = htons(port);

	// if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
	// 	std::cerr << "Error. Bind failed. " << strerror(errno) << std::endl;
	// 	return (1);
	// }

	// if (listen(server_fd, backlog) < 0){
	// 	std::cerr << "Error. Listen failed. " << strerror(errno) << std::endl;
	// 	return (1);
	// }
	// while(1){
	// 	std::cout << "Waiting for connection" << std::endl;
	// 	//accpet second parmater for optional client only 
	// 	int new_socket = accept(server_fd,(struct sockaddr *)&address,
	// 	 (socklen_t *)&addrlen);
	// 	if (new_socket < 0){
	// 		std::cerr << "Error. Accept failed. " << strerror(errno) << std::endl;
	// 		return (1);
	// 	}
	// 	char buffer[20000] = {0};
	// 	int valread = read(new_socket, buffer, 1024);
	// 	std::cout << "From client" << std::endl;
	// 	std::cout << buffer << std::endl;
	// 	if (valread < 0){
	// 		std::cout << "Error. Read failed. " << strerror(errno) << std::endl;
	// 		return (1);
	// 	}
	// 	//Header
	// 	std::string server_msg = "HTTP/1.1 200 OK\n";
	// 	server_msg + "Content-Type: text/plain\n";
	// 	server_msg + "content-Length: 20\n";

	// 	//Body
	// 	server_msg + "\nHello World!\n";
		
	// 	write(new_socket, server_msg.c_str(), server_msg.size());
	// 	std::cout << "Server message sent" << std::endl;
	// 	close(new_socket);
	// }
	

}


