/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/03 17:53:08 by zah               #+#    #+#             */
/*   Updated: 2023/07/06 20:50:39 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Webserv.hpp"
# include <vector>

# define BUFFER_SIZE 1000

struct serverConf{
	std::string hostName;
	std::string	port;
};

class Server{
	private:
		serverConf			_serverConf;
		int					socket_fd;
	
	public:
		Server();
		~Server();
		int				init();
		void			run();
		std::string		receiveAll(std::string &msg, int socket_fd);
		
};

std::ostream&	operator<<(std::ostream& os, const serverConf& obj);

#endif