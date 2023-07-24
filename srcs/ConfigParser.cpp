/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zah <zah@student.42kl.edu.my>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/06 11:10:16 by zah               #+#    #+#             */
/*   Updated: 2023/07/06 21:03:51 by zah              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

ConfigParser::ConfigParser(){
	this->delimiter = " \r\t\n";
	this->specialChar = ";{}";
	this->tokenizer = Tokenizer(this->delimiter, this->specialChar);
}

ConfigParser::~ConfigParser(){
	// for(size_t i = 0; i < this->serverConfs.size(); i++){
	// 	delete (&this->serverConfs[i]);
	// }
	while (!serverConfs.empty()) {
    	delete serverConfs.back(); // Delete the last dynamically allocated struct
   		serverConfs.pop_back();    // Remove the pointer from the vector
	}
}

int	ConfigParser::parse(std::string &path){
	std::ifstream	file(path.c_str());

	if (!file.is_open()){
		std::cout << COLOR_RED << "Error. Failed to open file" << path << COLOR_RESET << std::endl;
		return (0);
	}
	std::stringstream	buffer;

	buffer << file.rdbuf();
	this->tokens = this->tokenizer.Tokenize(buffer.str());
	if (parseToken() == 0)
		return (0);
	file.close();
	return (1);
}


int	ConfigParser::parseToken(){
	int			indent_level = 0;
	serverConf	*current_conf = NULL;

	for (size_t i = 0; i < this->tokens.size(); i++){
		if (tokens[i] == "{"){
			indent_level += 1;
			continue ;
		}
		if (tokens[i] == "}"){
			indent_level -= 1;
			continue ;
		}
		if (this->tokens[i] == "server"){
			if (current_conf != NULL){
				this->serverConfs.push_back(current_conf);
			}
			else
				current_conf = new serverConf;
			continue ;
		}
		if (this->tokens[i] == "location"){
			if (parseLocation(i, indent_level, current_conf) == 0)
				return (0);
		}
		else{
			if (parseServer(i, indent_level, current_conf) == 0)
				return (0);
		}
	}
	if (current_conf != NULL){
		this->serverConfs.push_back(current_conf);
	}
	return (1);
}

int	ConfigParser::parseServer(size_t &current, int indent_level, serverConf *current_conf){
	std::string	keys[] = {"listen", "server_name", "root"};
	const int			key_size = sizeof(keys) / sizeof(keys[0]);
	typedef int (ConfigParser::*func)(size_t &, serverConf *);
	func	key_funcs[key_size] = {&ConfigParser::parseListen, &ConfigParser::parseServerName, &ConfigParser::parseRoot};

	//Check present of server block
	if (current_conf == NULL){
		std::cout << COLOR_RED << "Error. Server block not found when parsing " << this->tokens[current] << COLOR_RESET << std::endl;
		return (0);
	}

	//Check indent level
	if (indent_level != 1){
		std::cout << COLOR_RED << "Error. Invalid indent level when parsing " << this->tokens[current] << COLOR_RESET << std::endl;
		return (0);
	}

	//Check keyword match
	int keyword_found = -1;

	for (int i=0; i < key_size; i++){
		if (this->tokens[current] == keys[i]){
			keyword_found = i;
			break ;
		}
	}
	if (keyword_found == -1){
		std::cout << COLOR_RED << "Error. Invalid keyword when parsing " << this->tokens[current] << COLOR_RESET << std::endl;
		return (0);
	}

	return (this->*key_funcs[keyword_found])(current, current_conf);
	
	return 1;
}

int	ConfigParser::parseLocation(size_t &current, int indent_level, serverConf *currentConf){
	(void) current;
	(void) indent_level;
	(void) currentConf;
	return 1;
}

int	ConfigParser::parseListen(size_t &current, serverConf *current_conf){
	current += 1;
	if (!current_conf->host.empty()){
		std::cout << COLOR_RED << "Error. Multiple host present" << COLOR_RESET << std::endl;
		return (0);
	}
	std::string::size_type port_pos = this->tokens[current].find(":");
	if (port_pos == std::string::npos){
		std::cout << COLOR_RED << "Error. No port number specified: " << this->tokens[current] << COLOR_RESET << std::endl;
		return (0);
	}
	current_conf->host = this->tokens[current].substr(0, port_pos);
	current_conf->port_number.push_back(this->tokens[current].substr(port_pos + 1));
	current += 1;
	
	while (this->tokens[current] != ";"){
		//Check if is full address
		port_pos = this->tokens[current].find(":");
		if (port_pos == std::string::npos)
			current_conf->port_number.push_back(this->tokens[current]);
		else{
			std::string host = this->tokens[current].substr(0, port_pos);
			if (host != current_conf->host){
				std::cout << COLOR_RED << "Error. Host address does not match " << this->tokens[current] << COLOR_RESET << std::endl;
				return (0);
			}
			current_conf->port_number.push_back(this->tokens[current].substr(port_pos + 1));
		}
		current +=1;
	}
	return (1);
}

int ConfigParser::parseServerName(size_t &current, serverConf *current_conf){
	current += 1;
	current_conf->server_name = this->tokens[current];
	current += 1;
	if (this->tokens[current] != ";"){
		std::cout << COLOR_RED << "Error: missing ending character ; after " << this->tokens[current - 1] << COLOR_RESET << std::endl;
		return (0);
	}
	return (1);
}

int ConfigParser::parseRoot(size_t &current, serverConf *current_conf){
	current += 1;
	current_conf->root = this->tokens[current];
	current += 1;
	if (this->tokens[current] != ";"){
		std::cout << COLOR_RED << "Error: missing ending character ; after " << this->tokens[current - 1] << COLOR_RESET << std::endl;
		return (0);
	}
	return (1);
}

void	ConfigParser::printConf(){
	for (size_t i = 0; i < this->serverConfs.size(); i++){
		std::cout << "Host: " << this->serverConfs[i]->host << std::endl;
		std::cout << "Port: " ;
		for (size_t j=0; j < this->serverConfs[i]->port_number.size(); j++){
			std::cout << this->serverConfs[i]->port_number[j] << ", ";
		}
		std::cout << std::endl;
		std::cout << "Server Name: " << this->serverConfs[i]->server_name << std::endl;
		std::cout << "Root: " << this->serverConfs[i]->root << std::endl;
	}
}