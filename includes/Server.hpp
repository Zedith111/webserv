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

# include "utils.hpp"
# include "Webserv.hpp"
# include "serverUtils.hpp"

# include <vector>
# include <sys/select.h>

# define BUFFER_SIZE 1024

struct requestData{
	int server_fd;
	int status_code;
	std::string whole_request;
	std::string header;
	METHOD method;
	std::string path;
	std::string file_path;
	std::string body;
	std::string contentLength;
};

class Server{
	private:
		std::vector<serverConf *>	confs;
		std::map<int, serverConf>	servers;
		fd_set						read_fd, write_fd;
		std::map<int, requestData>	client_requests;
		std::map<int, std::string>	client_responses;
		std::map<int, std::string>	reason_phrases;
		std::ofstream				database;
		
		int				acceptNewConnection(int socket_fd);
		void			handleConnection(int socket_fd);
		void			handleRequest(int socket_fd);
		void			sendResponse(int socket_fd);
		int				checkReceive(std::string &msg);
		std::string		handleError(int status_code, int server_fd);
		int				checkHost(std::string &header, std::string &server_name);

		std::string		handleGet(int &client_fd, locationInfo &location);
		std::string		handlePost(int &client_fd, locationInfo &location);
		std::string		handlePut(int &client_fd, locationInfo &location);
		std::string		handleHead(int &client_fd, locationInfo &location);
		std::string		handleDelete(int &client_fd, locationInfo &location);

		std::string		checkDirectoryRoute(int server_fd, std::string &path);
		std::string		generateAutoindex(int &client_fd, std::string &route, std::string &file_path);
		
	public:
		Server();
		~Server();
		int				init(std::vector<serverConf *> confs);
		void			run();
		
};



#endif