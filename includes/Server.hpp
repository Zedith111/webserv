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
# include <sys/wait.h>
# include <cstdio>


// # define BUFFER_SIZE 10000
# define BUFFER_SIZE 1024

/**
 * @brief Structure to store the request data received from the client.
 * @param server_fd The file descriptor of the server which responsible to handle the client request.
 * @param status_code The status code of the request.
 * @param port The port number of the server where client sent request.
 * @param whole_request The whole request received from the client.
 * @param header The header of the request.
 * @param method The method of the request.
 * @param route The url route of the request.
 * @param file_path The file path of the request excluding the route.For example localhost/dir/test.pdf, 
 * file path will be test.pdf.
 * @param body The body of the request.
 * @param contentLength The content length of the request.
 * 
 * The following param will only be used when cgi request detected.
 * @param is_cgi Whether the request is cgi request. This will affect the sending process as no header will 
 * be added
 * @param query_string The query string of the cgi request, will only be used when GET.
 * @param envp The environment variable which will be pass through when execute the cgi.
 */
struct requestData{
	int server_fd;
	int status_code;
	std::string port;
	std::string whole_request;
	std::string header;
	METHOD method;
	std::string route;
	std::string file_path;
	std::string body;
	std::string::size_type contentLength;

	int is_cgi;
	std::string interpretor;
	std::string query_string;
	char **envp;
};



class Server{
	private:
		std::vector<serverConf *>	confs;
		std::map<int, serverConf>	servers;
		fd_set						read_fd, write_fd;
		std::map<int, std::string>	sockets_port;
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

		std::string 	handleCGI(int &client_fd);

		std::string		handleGet(int &client_fd, locationInfo &location);
		std::string		handlePost(int &client_fd, locationInfo &location);
		std::string		handlePut(int &client_fd, locationInfo &location);
		std::string		handleHead(int &client_fd, locationInfo &location);
		std::string		handleDelete(int &client_fd, locationInfo &location);

		std::string		checkDirectoryRoute(int server_fd, std::string &path);
		std::string		generateAutoindex(int &client_fd, std::string &route, std::string &file_path);
		std::string		handleUpload(int &client_fd, locationInfo &location, int method);
		std::string 	handlePostText(int &client_fd);
	public:
		Server();
		~Server();
		int				init(std::vector<serverConf *> confs);
		void			run();
		
};



#endif