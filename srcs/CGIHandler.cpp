/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 13:09:33 by zah               #+#    #+#             */
/*   Updated: 2023/09/15 16:23:21 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

void addEnv(std::string value, char **env){
	int i = 0;
	while (env[i] != NULL){
		i++;
	}
	env[i] = strdup(value.c_str());
}

char **createEnv(requestData &request, serverConf &conf){
	char **env = new char*[ENV_COUNT];

	std::memset(env, 0, sizeof(char *) * ENV_COUNT);

	addEnv("GATEWAY_INTERFACE=CGI/1.1", env);
	addEnv("SERVER_NAME=" + conf.server_name, env);
	addEnv("SERVER_SOFTWARE=webserv", env);
	addEnv("SERVER_PROTOCOL=HTTP/1.1", env);
	addEnv("SERVER_PORT=" + request.port, env);
	if (request.method == GET){
		addEnv("REQUEST_METHOD=GET", env);
		addEnv("QUERY_STRING=" + request.query_string, env);
	}
	else{
		addEnv("REQUEST_METHOD=POST", env);
		addEnv("QUERY_STRING=", env);
	}
	std::string whole_path = request.route + "/" + request.file_path;
	std::cout << "Whole path: " << whole_path << std::endl;
	addEnv("PATH_INFO=" + whole_path, env);
	addEnv("HTTP_X_SECRET_HEADER_FOR_TEST=1", env);
	addEnv("SCRIPT_NAME=" + request.interpretor, env);
	addEnv("DOCUMENT_ROOT=" + request.interpretor, env);
	addEnv("REMOTE_ADDR=127.0.0.1", env);
	addEnv("REDIRECT_STATUS=200", env);
	addEnv("CONTENT_TYPE=*/*", env);
	addEnv("CONTENT_LENGTH=" + intToString((int)request.contentLength), env);
	addEnv("REQUEST_URI=" + whole_path, env);
	return (env);
}

int handleCGI(requestData &request, locationInfo &location, std::string &response, serverConf &conf){
	std::string cgi_path = location.root + "/" + request.file_path;
	std::string interpretor = request.interpretor;

	if (access(cgi_path.c_str(), R_OK | X_OK) != 0){
		std::cout << COLOR_RED << "Error: Unable to access CGI script. " << cgi_path << COLOR_RESET << std::endl;
		response = handleError(500, conf);
		return (500);
	}

	if (access(interpretor.c_str(), R_OK | X_OK) != 0){
		std::cout << COLOR_RED << "Error: Unable to access CGI interpretor. " << interpretor << COLOR_RESET << std::endl;
		response = handleError(500, conf);
		return (500);
	}

	// if (DEBUG){
		std::cout << "CGI Path: " << cgi_path << std::endl;
		std::cout << "Interpretor: " << interpretor << std::endl;
	// }

	//Save stdin and stdout to reset back after 
	int dup_stdin = dup(STDIN_FILENO);
	int dup_stdout = dup(STDOUT_FILENO);

	FILE *tempIn = std::tmpfile();
	FILE *tempOut = std::tmpfile();
	int fdIn = fileno(tempIn);
	int fdOut = fileno(tempOut);

	// if (request.method == POST){
		std::fwrite(request.body.c_str(), 1, request.body.length(), tempIn);
	// }
	std::cout << "Body: " << request.body << std::endl;
	std::rewind(tempIn);

	pid_t pid = fork();
	if (pid < 0){
		std::cout << COLOR_RED << "Error: Unable to fork" << COLOR_RESET << std::endl;
		fclose(tempIn);
		fclose(tempOut);
		response = handleError(500, conf);
		return (500);
	}
	if (pid == 0){
		char **env = createEnv(request, conf);
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		char *argv[3];
		argv[0] = const_cast<char *>(interpretor.c_str());
		argv[1] = const_cast<char *>(cgi_path.c_str());
		argv[2] = NULL;
		execve(argv[0], argv, env);
		std::cout << COLOR_RED << "Error: Unable to execute CGI script. " << cgi_path << COLOR_RESET << std::endl;
		delete[] env;
		exit (1);
	}
	else{
		int status;
		waitpid(-1, &status, 0);
		if (WIFEXITED(status) == 0){
			std::cout << COLOR_RED << "Error: CGI script exited abnormally. " << cgi_path << COLOR_RESET << std::endl;
			response = handleError(500, conf);
			return (500);
		}
		int bytes_read = 1;
		char buffer[1024];
		
		std::rewind(tempOut);
		// lseek(fdOut, 0, SEEK_SET);
		while (bytes_read > 0){
			memset(buffer, 0, 1024);
			bytes_read = read(fdOut, buffer, 1024);
			response.append(buffer, bytes_read);
		}
	}
	std::cout << "Response: " << response << std::endl;
	dup2(dup_stdin, STDIN_FILENO);
	dup2(dup_stdout, STDOUT_FILENO);
	fclose(tempIn);
	fclose(tempOut);
	close(fdIn);
	close(fdOut);
	close(dup_stdin);
	close(dup_stdout);

	if (!pid)
		exit(0);
	return (200);
}

// int handleCGI(requestData &request, locationInfo &location, std::string &response, serverConf &conf){
// 	int child_pipe[2];
// 	int parent_pipe[2];

// 	std::string cgi_path = location.root + "/" + request.file_path;
// 	std::string interpretor = request.interpretor;

// 	if (access(cgi_path.c_str(), R_OK | X_OK) != 0){
// 		std::cout << COLOR_RED << "Error: Unable to access CGI script. " << cgi_path << COLOR_RESET << std::endl;
// 		response = handleError(500, conf);
// 		return (500);
// 	}

// 	if (access(interpretor.c_str(), R_OK | X_OK) != 0){
// 		interpretor = "." + interpretor;
// 		if (access(interpretor.c_str(), R_OK | X_OK) != 0){
// 			std::cout << COLOR_RED << "Error: Unable to access CGI interpretor. " << interpretor << COLOR_RESET << std::endl;
// 			response = handleError(500, conf);
// 			return (500);
// 		}
// 	}

// 	if (pipe(child_pipe) < 0){
// 		std::cout << COLOR_RED << "Error: Unable to create pipe" << COLOR_RESET << std::endl;
// 		response = handleError(500, conf);
// 		return (500);
// 	}
// 	if (pipe(parent_pipe) < 0){
// 		std::cout << COLOR_RED << "Error: Unable to create pipe" << COLOR_RESET << std::endl;
// 		response = handleError(500, conf);
// 		return (500);
// 	}

// 	int pid = fork();
// 	if (pid < 0){
// 		std::cout << COLOR_RED << "Error: Unable to fork" << COLOR_RESET << std::endl;
// 		response = handleError(500, conf);
// 		return (500);
// 	}
// 	if (pid == 0){
// 		char **env = createEnv(request, conf);

// 		char *argv[3];
// 		argv[0] = const_cast<char *>(interpretor.c_str());
// 		argv[1] = const_cast<char *>(cgi_path.c_str());
// 		argv[2] = NULL;

// 		close(child_pipe[1]);
// 		close(parent_pipe[0]);
		
// 		dup2(child_pipe[0], STDIN_FILENO);
// 		dup2(parent_pipe[1], STDOUT_FILENO);
// 		close(parent_pipe[1]);
// 		execve(argv[0], argv, env);
// 		std::cout << COLOR_RED << "Error: Unable to execute CGI script. " << cgi_path << COLOR_RESET << std::endl;
// 		delete[] env;
// 		exit (1);
// 	}
// 	else{
// 		close(child_pipe[0]);
// 		close(parent_pipe[1]);

// 		if (request.method == POST){
// 			std::cout << "Writing body to pipe" << std::endl;
// 			write(child_pipe[1], request.body.c_str(), request.body.length());
// 		}
// 		close(child_pipe[1]);

// 		std::ofstream outfile("temp", std::ios::out | std::ios::binary);
// 		if (!outfile.good()){
// 			std::cout << COLOR_RED << "Error: Unable to open temp file" << COLOR_RESET << std::endl;
// 			response = handleError(500, conf);
// 			return (500);
// 		}
// 		ssize_t bytes_read;
// 		char buffer[1024];
// 		while ((bytes_read = read(parent_pipe[0], buffer, 1024)) > 0){
// 			memset(buffer, 0, 1024);
// 			buffer[bytes_read] = '\0';
// 			outfile.write(buffer, bytes_read);
// 			response.append(buffer, bytes_read);
// 		}

// 		std::ifstream infile("temp", std::ios::in | std::ios::binary);
// 		std::string line;
// 		std::cout << "print infile" << std::endl;
//     	while (std::getline(infile, line)) {
//         std::cout << line << std::endl; 
//     	}

// 		std::cout << "Response: " << response << std::endl;
		

// 		close(parent_pipe[0]);
// 		outfile.close();
		
// 		int status;
// 		waitpid(-1, &status, 0);
// 		if (WIFEXITED(status) == 0){
// 			std::cout << COLOR_RED << "Error: CGI script exited abnormally. " << cgi_path << COLOR_RESET << std::endl;
// 			response = handleError(500, conf);
// 			return (500);
// 		}
// 		return (200);
// 	}
// }