/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 17:53:08 by zah               #+#    #+#             */
/*   Updated: 2023/07/09 12:46:10 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Webserv.hpp"
# include "serverUtils.hpp"
# include <vector>
# include <sys/select.h>

# define BUFFER_SIZE 1024

/**
 * choose port and host(host refer to ip address)
 * server_name (default to localhost)
 * if multiple server use same port and host. The first will be used
 * default error page
 * limit client body size
 * set up route
//  */
// struct serverConf{
// 	std::string hostName;
// 	std::string	port;
// 	int			totalPort;
// 	std::vector<int>	portNumber;
// };

struct requestData{
	std::string header;
	METHOD method;
	std::string path;
	std::string body;
	std::string contentLength;
};

class Server{
	private:
		serverConf					conf;
		std::vector<int>			socket_fds;
		fd_set						read_fd, write_fd;
		std::map<int, std::string>	client_requests;
		std::map<int, std::string>	client_responses;
		
		int				acceptNewConnection(int socket_fd);
		void			handleConnection(int socket_fd);
		void			handleRequest(int socket_fd);
		void			sendResponse(int socket_fd);
		int				checkReceive(std::string &msg);

		std::string		handleGet(std::ifstream &file);
		std::string		handlePost(std::ifstream &file);
		std::string		handleHead(std::ifstream &file);
		std::string		handleDelete(std::ifstream &file);
		
	public:
		Server();
		~Server();
		int				init();
		void			run();
		
};

std::ostream&	operator<<(std::ostream& os, const serverConf& obj);

#endif