/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/13 13:09:33 by zah               #+#    #+#             */
/*   Updated: 2023/09/13 18:00:51 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RequestHandler.hpp"

// char **createEnv(requestData &request, serverConf &conf){
// 	char **env = new char*[ENV_COUNT];

// 	//if (request.method == GET)


// }

// int handleCGI(requestData &request, locationInfo &location, std::string &response, serverConf &conf){
// 	std::cout << "Handle CGI" << std::endl;

// 	std::string cgi_path = location.root + "/" + request.file_path;
// 	std::string interpretor = request.interpretor;

// 	std::cout << "Interpretor: " << interpretor << std::endl;
// 	std::cout << "CGI Path: " << cgi_path << std::endl;

// 	if (access(cgi_path.c_str(), R_OK | X_OK) != 0){
// 		std::cout << COLOR_RED << "Error: Unable to access CGI script. " << cgi_path << COLOR_RESET << std::endl;
// 		response = handleError(500, conf);
// 		return (500);
// 	}

// 	if (access(interpretor.c_str(), R_OK | X_OK) != 0){
// 		interpretor = "." + interpretor;
// 		std::cout << "New interpretor: " << interpretor << std::endl;
// 		if (access(interpretor.c_str(), R_OK | X_OK) != 0){
// 			std::cout << COLOR_RED << "Error: Unable to access CGI interpretor. " << interpretor << COLOR_RESET << std::endl;
// 			response = handleError(500, conf);
// 			return (500);
// 		}
// 	}

// 	int stdin = dup(STDIN_FILENO);
// 	int stdout = dup(STDOUT_FILENO);

// 	//tempIn is used to store the duplicate script, tempOut is used to store the output
// 	FILE *tempIn = std::tmpfile();
// 	FILE *tempOut = std::tmpfile();
// 	int fdIn = fileno(tempIn);
// 	int fdOut = fileno(tempOut);
// 	std::string res;

// 	FILE *script = fopen(cgi_path.c_str(), "r");
// 	if (script == NULL){
// 		std::cout << COLOR_RED << "Error: Unable to open CGI script. " << cgi_path << COLOR_RESET << std::endl;
// 		response = handleError(500, conf);
// 		return (500);
// 	}

// 	char buffer[1024];
// 	int bytes_read;
// 	while ((bytes_read = std::fread(buffer, 1, 1024, script)) > 0){
// 		std::fwrite(buffer, 1, bytes_read, tempIn);
// 	}
// 	fclose(script);
// 	std::rewind(tempIn);

// 	pid_t child = fork();
// 	if (child < 0){
// 		std::cout << COLOR_RED << "Error: Unable to fork" << COLOR_RESET << std::endl;
// 		fclose(tempIn);
// 		fclose(tempOut);
// 		response = handleError(500, conf);
// 		return (500);
// 	}
// 	if (child == 0){
// 		dup2(fdIn, STDIN_FILENO);
// 		dup2(fdOut, STDOUT_FILENO);
// 		char *argv[3];
// 		argv[0] = const_cast<char *>(interpretor.c_str());
// 		argv[1] = strdup(cgi_path.c_str());
// 		argv[2] = NULL;

// 		execve(argv[0], argv, NULL);
// 	}
// 	else{
// 		int status;
// 		waitpid(child, &status, 0);
// 		char buffer[1024];
// 		std::rewind(tempOut);
// 		while ((bytes_read = std::fread(buffer, 1, 1024, tempOut)) > 0){
// 			res.append(buffer, bytes_read);
// 		}
// 	}
// 	dup2(stdin, STDIN_FILENO);
// 	dup2(stdout, STDOUT_FILENO);
// 	fclose(tempIn);
// 	fclose(tempOut);
// 	response = res;
// 	return (200);
// 	// if (child == 0){
// 	// 	dup2(fdIn, STDIN_FILENO);
// 	// 	dup2(fdOut, STDOUT_FILENO);
// 	// 	request.envp = createEnv(request, conf);
// 	// 	close(fdIn);
// 	// 	close(fdOut);
// 	// 	char **env = createEnv(request, conf);
// 	// 	execve(interpretor.c_str(), env, NULL);
// 	// }
// 	// else{
// 	// 	waitpid(child, NULL, 0);
// 	// 	std::rewind(tempOut);
// 	// 	while ((bytes_read = std::fread(buffer, 1, 1024, tempOut)) > 0){
// 	// 		res.append(buffer, bytes_read);
// 	// 	}
// 	// 	response = res;
// 	// 	std::fclose(tempIn);
// 	// 	std::fclose(tempOut);
// 	// 	dup2(stdin, STDIN_FILENO);
// 	// 	dup2(stdout, STDOUT_FILENO);
// 	// 	close(stdin);
// 	// 	close(stdout);
// 	// 	return (200);
// 	// }
	
// }

//Check
//GET need to store query inside tempIn?
//need Rewind?
//Need duplicate tempIn?
int handleCGI(requestData &request, locationInfo &location, std::string &response, serverConf &conf){
	std::cout << "Handle CGI" << std::endl;

	std::string cgi_path = location.root + "/" + request.file_path;
	std::string interpretor = request.interpretor;

	std::cout << "Interpretor: " << interpretor << std::endl;
	std::cout << "CGI Path: " << cgi_path << std::endl;

	if (access(cgi_path.c_str(), R_OK | X_OK) != 0){
		std::cout << COLOR_RED << "Error: Unable to access CGI script. " << cgi_path << COLOR_RESET << std::endl;
		response = handleError(500, conf);
		return (500);
	}

	if (access(interpretor.c_str(), R_OK | X_OK) != 0){
		interpretor = "." + interpretor;
		std::cout << "New interpretor: " << interpretor << std::endl;
		if (access(interpretor.c_str(), R_OK | X_OK) != 0){
			std::cout << COLOR_RED << "Error: Unable to access CGI interpretor. " << interpretor << COLOR_RESET << std::endl;
			response = handleError(500, conf);
			return (500);
		}
	}


	int stdin = dup(STDIN_FILENO);
	int stdout = dup(STDOUT_FILENO);

	FILE *tempIn = std::tmpfile();
	FILE *tempOut = std::tmpfile();
	int fdIn = fileno(tempIn);
	int fdOut = fileno(tempOut);

	if (request.method == POST){
		std::fwrite(request.body.c_str(), 1, request.body.length(), tempIn);
	}
	else{
		std::string query = request.query_string;
		std::fwrite(query.c_str(), 1, query.length(), tempIn);
	}
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
		int dupIn = dup(fdIn);
		int dupOut = dup(fdOut);
		dup2(dupIn, STDIN_FILENO);
		close(dupIn);
		dup2(dupOut, STDOUT_FILENO);
		close(dupOut);
		char *argv[3];
		argv[0] = const_cast<char *>(interpretor.c_str());
		argv[1] = strdup(cgi_path.c_str());
		argv[2] = NULL;

		execve(argv[0], argv, NULL);
		close(fdIn);
		close(fdOut);
		std::cout << COLOR_RED << "Error: Unable to execute CGI script. " << cgi_path << COLOR_RESET << std::endl;
		exit(1);
	}
	else{
		int status;
		waitpid(-1, &status, 0);
		if (WIFEXITED(status) == 0){
			std::cout << COLOR_RED << "Error: CGI script exited abnormally. " << cgi_path << COLOR_RESET << std::endl;
			response = handleError(500, conf);
			return (500);
		}
		char buffer[1024];
		std::rewind(tempOut);
		int bytes_read;
		while ((bytes_read = std::fread(buffer, 1, 1024, tempOut)) > 0){
			response.append(buffer, bytes_read);
		}
	}
	std::string::size_type pos = response.find("\r\n\r\n");
	if (pos == std::string::npos){
		std::cout << COLOR_RED << "Error: CGI script did not return a valid response. " << cgi_path << COLOR_RESET << std::endl;
		fclose(tempIn);
		fclose(tempOut);
		std::cout << "Response: " << response << std::endl;
		response = handleError(404, conf);
		return (404);
	}
	else{
		std::cout << "CGI success" << std::endl;
		fclose(tempIn);
		fclose(tempOut);
		dup2(stdin, STDIN_FILENO);
		close(stdin);
		dup2(stdout, STDOUT_FILENO);
		close(stdout);
		return (200);

	}
	// if (child == 0){
	// 	dup2(fdIn, STDIN_FILENO);
	// 	dup2(fdOut, STDOUT_FILENO);
	// 	request.envp = createEnv(request, conf);
	// 	close(fdIn);
	// 	close(fdOut);
	// 	char **env = createEnv(request, conf);
	// 	execve(interpretor.c_str(), env, NULL);
	// }
	// else{
	// 	waitpid(child, NULL, 0);
	// 	std::rewind(tempOut);
	// 	while ((bytes_read = std::fread(buffer, 1, 1024, tempOut)) > 0){
	// 		res.append(buffer, bytes_read);
	// 	}
	// 	response = res;
	// 	std::fclose(tempIn);
	// 	std::fclose(tempOut);
	// 	dup2(stdin, STDIN_FILENO);
	// 	dup2(stdout, STDOUT_FILENO);
	// 	close(stdin);
	// 	close(stdout);
	// 	return (200);
	// }
	
}