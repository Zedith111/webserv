/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 13:09:33 by zah               #+#    #+#             */
/*   Updated: 2023/09/14 20:49:18 by zah              ###   ########.fr       */
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
	addEnv("SERVER_SOFTWARE=Webserv/1.0", env);
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
	addEnv("PATH_TRANSLATED=" + whole_path, env);
	addEnv("SCRIPT_NAME=" + request.interpretor, env);
	addEnv("DOCUMENT_ROOT=" + request.interpretor, env);
	addEnv("REMOTE_ADDR=127.0.0.1", env);
	addEnv("REDIRECT_STATUS=200", env);
	addEnv("CONTENT_TYPE=*/*", env);
	addEnv("CONTENT_LENGTH=" + std::to_string(request.contentLength), env);

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
		interpretor = "." + interpretor;
		if (access(interpretor.c_str(), R_OK | X_OK) != 0){
			std::cout << COLOR_RED << "Error: Unable to access CGI interpretor. " << interpretor << COLOR_RESET << std::endl;
			response = handleError(500, conf);
			return (500);
		}
	}
	if (DEBUG){
		std::cout << "CGI Path: " << cgi_path << std::endl;
		std::cout << "Interpretor: " << interpretor << std::endl;
	}

	//Save stdin and stdout to reset back after 
	int dup_stdin = dup(STDIN_FILENO);
	int dup_stdout = dup(STDOUT_FILENO);

	FILE *tempIn = std::tmpfile();
	FILE *tempOut = std::tmpfile();
	int fdIn = fileno(tempIn);
	int fdOut = fileno(tempOut);

	if (request.method == POST){
		std::fwrite(request.body.c_str(), 1, request.body.length(), tempIn);
	}
	std::rewind(tempIn);

	char **env = createEnv(request, conf);
	// for (int i =0; i < ENV_COUNT-1; i++){
	// 	std::cout << env[i] << std::endl;
	// }

	pid_t pid = fork();
	if (pid < 0){
		std::cout << COLOR_RED << "Error: Unable to fork" << COLOR_RESET << std::endl;
		fclose(tempIn);
		fclose(tempOut);
		response = handleError(500, conf);
		return (500);
	}
	if (pid == 0){
		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		char *argv[3];
		argv[0] = const_cast<char *>(interpretor.c_str());
		argv[1] = strdup(cgi_path.c_str());
		argv[2] = NULL;
		execve(argv[0], argv, env);
		std::cout << COLOR_RED << "Error: Unable to execute CGI script. " << cgi_path << COLOR_RESET << std::endl;
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
	// std::cout << "Response: " << response << std::endl;
	dup2(dup_stdin, STDIN_FILENO);
	dup2(dup_stdout, STDOUT_FILENO);
	fclose(tempIn);
	fclose(tempOut);
	close(fdIn);
	close(fdOut);
	close(dup_stdin);
	close(dup_stdout);
	delete[] env;

	if (!pid)
		exit(0);
	return (200);
}


// int handleCGI(requestData &request, locationInfo &location, std::string &response, serverConf &conf){
// 	std::cout << "Handle CGI" << std::endl;

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

// 	int stdin = dup(STDIN_FILENO);
// 	int stdout = dup(STDOUT_FILENO);

// 	FILE *tempIn = std::tmpfile();
// 	FILE *tempOut = std::tmpfile();
// 	int fdIn = fileno(tempIn);
// 	int fdOut = fileno(tempOut);

// 	if (request.method == POST){
// 		std::fwrite(request.body.c_str(), 1, request.body.length(), tempIn);
// 	}
// 	// else{
// 	// 	std::string query = request.query_string;
// 	// 	std::fwrite(query.c_str(), 1, query.length(), tempIn);
// 	// }
// 	std::rewind(tempIn);

// 	char **env = createEnv(request, conf);

// 	pid_t pid = fork();
// 	if (pid < 0){
// 		std::cout << COLOR_RED << "Error: Unable to fork" << COLOR_RESET << std::endl;
// 		fclose(tempIn);
// 		fclose(tempOut);
// 		response = handleError(500, conf);
// 		return (500);
// 	}
// 	if (pid == 0){
// 		int dupIn = dup(fdIn);
// 		int dupOut = dup(fdOut);
// 		dup2(dupIn, STDIN_FILENO);
// 		close(dupIn);
// 		dup2(dupOut, STDOUT_FILENO);
// 		close(dupOut);
// 		char *argv[3];
// 		argv[0] = const_cast<char *>(interpretor.c_str());
// 		argv[1] = strdup(cgi_path.c_str());
// 		argv[2] = NULL;
// 		execve(argv[0], argv, env);
// 		close(fdIn);
// 		close(fdOut);
// 		std::cout << COLOR_RED << "Error: Unable to execute CGI script. " << cgi_path << COLOR_RESET << std::endl;
// 		exit(1);
// 	}
// 	else{
// 		int status;
// 		waitpid(-1, &status, 0);
// 		if (WIFEXITED(status) == 0){
// 			std::cout << COLOR_RED << "Error: CGI script exited abnormally. " << cgi_path << COLOR_RESET << std::endl;
// 			response = handleError(500, conf);
// 			return (500);
// 		}
// 		char buffer[1024];
// 		std::rewind(tempOut);
// 		int bytes_read;
// 		while ((bytes_read = std::fread(buffer, 1, 1024, tempOut)) > 0){
// 			response.append(buffer, bytes_read);
// 		}
// 	}
// 	std::string::size_type pos = response.find("\r\n\r\n");
// 	if (pos == std::string::npos){
// 		std::cout << COLOR_RED << "Error: CGI script did not return a valid response. " << cgi_path << COLOR_RESET << std::endl;
// 		fclose(tempIn);
// 		fclose(tempOut);
// 		std::cout << "Response: " << response << std::endl;
// 		response = handleError(404, conf);
// 		request.status_code = 200;
// 		request.is_cgi = 0;
// 		return (200);
// 	}
// 	else{
// 		std::cout << "CGI success" << std::endl;
// 		fclose(tempIn);
// 		fclose(tempOut);
// 		dup2(stdin, STDIN_FILENO);
// 		close(stdin);
// 		dup2(stdout, STDOUT_FILENO);
// 		close(stdout);
// 		return (200);
// 	}
// }