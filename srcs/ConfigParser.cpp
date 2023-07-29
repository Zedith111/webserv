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
	this->special_char = ";{}";
	this->tokenizer = Tokenizer(this->delimiter, this->special_char);
}

ConfigParser::~ConfigParser(){
	std::cout << "Config Parser Destructor called" << std::endl;
	for (size_t i = 0; i < this->server_confs.size(); i++){
		serverConf *current = this->server_confs[i];
		if (current != NULL){
			std::map<std::string, locationInfo *>::iterator it;
			for (it = current->locations.begin(); it != current->locations.end();){
				delete it->second;
				it = current->locations.erase(it);
			}
		}
		delete current;
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
				this->server_confs.push_back(current_conf);
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
		this->server_confs.push_back(current_conf);
	}
	if (indent_level != 0){
		std::cout << COLOR_RED << "Error. Block not enclosed properly" << COLOR_RESET << std::endl;
		return (0);
	}
	std::cout << "Finish" << std::endl;
	return (1);
}

int	ConfigParser::parseServer(size_t &current, int indent_level, serverConf *current_conf){
	std::string	keys[] = {"listen", "server_name", "root", "error_pages"};
	const int			key_size = sizeof(keys) / sizeof(keys[0]);
	typedef int (ConfigParser::*func)(size_t &, serverConf *);
	func	key_funcs[key_size] = {&ConfigParser::parseListen, &ConfigParser::parseServerName, &ConfigParser::parseRoot, 
					&ConfigParser::parseErrorPages};

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

	for (int i = 0; i < key_size; i ++){
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
	//Check present of server block
	if (currentConf == NULL){
		std::cout << COLOR_RED << "Error. Server block not found when parsing " << this->tokens[current] << COLOR_RESET << std::endl;
		return (0);
	}

	//Check current indent level
	if (indent_level != 1){
		std::cout << COLOR_RED << "Error. Invalid indent level when parsing " << this->tokens[current] << COLOR_RESET << std::endl;
		return (0);
	}

	locationInfo *new_location = new locationInfo;
	current += 1;
	std::string route = this->tokens[current];

	current += 1;
	if (this->tokens[current] != "{"){
		std::cout << COLOR_RED << "Error. Missing { after location path" << COLOR_RESET << std::endl;
		return (0);
	}
	current += 1;
	std::string keys[] = {"autoindex", "root", "index", "limit_except"};
	const int key_size = sizeof(keys) / sizeof(keys[0]);
	typedef int (ConfigParser::*func)(size_t &, locationInfo *);
	func	key_funcs[key_size] = {&ConfigParser::parseAutoindex, &ConfigParser::parseLocationRoot, &ConfigParser::parseLocationIndex, &ConfigParser::parseLimitExcept};
	
	while (this->tokens[current] != "}"){
		int keyword_found = -1;
		for (int i = 0; i < key_size; i ++){
			if (this->tokens[current] == keys[i]){
				keyword_found = i;
				break ;
			}
		}
		if (keyword_found == -1){
			std::cout << COLOR_RED << "Error. Invalid keyword when parsing " << this->tokens[current] << COLOR_RESET << std::endl;
			return (0);
		}
		if ((this->*key_funcs[keyword_found])(current, new_location) == 0)
			return (0);
		current += 1;
	}
	currentConf->locations[route] = new_location;
	return 1;
}

/**
 * @brief Check and fill out necessary information if the config file does nor provide.
 * This includes:
 * 		- Necessary information, including host, port,root, server_name
 * 		- Check the error pages, if one of missing, fill out with default error pages
 * 		- Attach server root of location root 
 * Will return error if:
 * 		- Missing necessary information
 * 		- Invalid directory of location root
 */
int ConfigParser::validateConfig(){
	for (size_t i = 0; i < this->server_confs.size(); i++){
		serverConf *current_conf = this->server_confs[i];
		if (current_conf->host.empty() || current_conf->port_number.empty() || current_conf->root.empty() || current_conf->server_name.empty()){
			std::cout << COLOR_RED << "Error. Missing necessary information in server block" << COLOR_RESET << std::endl;
			return (0);
		}
		if (validateLocationRoot(current_conf) == 0)
			return (0);
		addErrorpages(current_conf);
	}
	return (1);
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
	return (checkEnding(current));
}

int ConfigParser::parseRoot(size_t &current, serverConf *current_conf){
	current += 1;
	std::string path = this->tokens[current];
	if (checkIsDirectory(path) != 1){
		std::cout << COLOR_RED << "Error. Invalid root path: " << path << COLOR_RESET << std::endl;
		return (0);
	}
	current_conf->root = path;
	current += 1;
	return (checkEnding(current));
}

int ConfigParser::parseErrorPages(size_t &current, serverConf *current_conf){
	current += 1;
	std::istringstream iss(this->tokens[current]);
	int error_code;
	if (!(iss >> error_code)){
		std::cout << COLOR_RED << "Error. Invalid error code: " << this->tokens[current] << COLOR_RESET << std::endl;
		return (0);
	}
	current += 1;
	if (checkIsDirectory(this->tokens[current]) != 0){
		std::cout << COLOR_RED << "Error. Invalid error page path: " << this->tokens[current] << COLOR_RESET << std::endl;
		return (0);
	}
	current_conf->error_pages[error_code] = this->tokens[current];
	current +=1;
	return (checkEnding(current));
}

int	ConfigParser::parseAutoindex(size_t &current, locationInfo *current_loc){
	current += 1;
	if (this->tokens[current] == "on")
		current_loc->autoindex = true;
	else if (this->tokens[current] == "off")
		current_loc->autoindex = false;
	else{
		std::cout << COLOR_RED << "Error. Invalid value of autoindex: " << this->tokens[current] << COLOR_RESET << std::endl;
		return (0);
	}
	current += 1;
	return (checkEnding(current));
}

int ConfigParser::parseLocationRoot(size_t &current, locationInfo *current_loc){
	current += 1;
	current_loc->root = this->tokens[current];
	current += 1;
	return (checkEnding(current));
}

int	ConfigParser::parseLocationIndex(size_t &current, locationInfo *current_loc){
	current += 1;
	current_loc->index = this->tokens[current];
	current += 1;
	return (checkEnding(current));
}

int	ConfigParser::parseLimitExcept(size_t &current, locationInfo *current_loc){
	current += 1;
	while (this->tokens[current] != ";")
	{
		if (this->tokens[current] == "GET" || this->tokens[current] == "POST" || this->tokens[current] == "PUT" ||
		this->tokens[current] == "HEAD" || this->tokens[current] == "DELETE"){
			current_loc->limit_except.push_back(this->tokens[current]);
		}
		else
		{
			std::cout << COLOR_RED << "Error. Invalid limit_except value: " << this->tokens[current] << COLOR_RESET << std::endl;
			return (0);
		}
		current += 1;
	}
	return (1); 
}

int ConfigParser::initDefaultErrorpages(){
	this->default_error_pages[404] = "./default_error_pages/404.html";
	this->default_error_pages[405] = "./default_error_pages/405.html";

	std::map<int, std::string>::iterator it;
	for (it = this->default_error_pages.begin(); it != this->default_error_pages.end(); it++){
		if (checkIsDirectory(it->second) != 0){
			std::cout << COLOR_RED << "Error. Invalid default error page path: " << it->second << COLOR_RESET << std::endl;
			return (0);
		}
		std::ifstream file(it->second);
		if (!file.is_open()){
			std::cout << COLOR_RED << "Error. Cannot open default error page: " << it->second << COLOR_RESET << std::endl;
			return (0);
		}
		file.close();
	}
	return (1);
}

void ConfigParser::addErrorpages(serverConf *current_conf){
	int all_codes[] = {404, 405};
	const int codes_size = sizeof(all_codes) / sizeof(int);

	for (int i = 0; i < codes_size; i++){
		if (current_conf->error_pages.find(all_codes[i]) == current_conf->error_pages.end()){
			current_conf->error_pages[all_codes[i]] = this->default_error_pages[all_codes[i]];
		}
	}
}

int ConfigParser::validateLocationRoot(serverConf *current_conf){
	std::map<std::string, locationInfo*>::iterator it;
	for (it = current_conf->locations.begin(); it != current_conf->locations.end(); it++){
		if (!it->second->root.empty()){
			std::string full_path = current_conf->root + it->second->root;
			if (checkIsDirectory(full_path) != 1){
				std::cout << COLOR_RED << "Error. Invalid location root path: " << full_path << COLOR_RESET << std::endl;
				return (0);
			}
			it->second->root = full_path;
		}
	}
	return (1);
}

int ConfigParser::checkEnding(size_t &current){
	if (this->tokens[current] != ";"){
		std::cout << COLOR_RED << "Error: missing ending character ; after " << this->tokens[current - 1] << COLOR_RESET << std::endl;
		return (0);
	}
	return (1); 
}

void	ConfigParser::printConf(){
	for (size_t i = 0; i < this->server_confs.size(); i++){
		std::cout << "Host: " << this->server_confs[i]->host << std::endl;
		std::cout << "Port: " ;
		for (size_t j=0; j < this->server_confs[i]->port_number.size(); j++){
			std::cout << this->server_confs[i]->port_number[j] << ", ";
		}
		std::cout << std::endl;
		std::cout << "Server Name: " << this->server_confs[i]->server_name << std::endl;
		std::cout << "Root: " << this->server_confs[i]->root << std::endl;
		std::cout << "Error Pages: " << std::endl;
		std::map<int, std::string>::iterator error_iter;
		for(error_iter=this->server_confs[i]->error_pages.begin(); error_iter!=this->server_confs[i]->error_pages.end(); ++error_iter){
			std::cout << "\t" << error_iter->first << ": " << error_iter->second << std::endl;
		}
		std::map<std::string, locationInfo *>::iterator loc_iter;
		for(loc_iter=this->server_confs[i]->locations.begin(); loc_iter!=this->server_confs[i]->locations.end(); ++loc_iter){
			std::cout << "Route: " << loc_iter->first << std::endl;
			std::cout << "\tRoot: " << loc_iter->second->root << std::endl;
			std::cout << "\tIndex: " << loc_iter->second->index << std::endl;
			std::cout << "\tAutoindex: " << loc_iter->second->autoindex << std::endl;
			std::cout << "\tLimit Except: ";
			for (size_t j=0; j < loc_iter->second->limit_except.size(); j++){
				std::cout << loc_iter->second->limit_except[j] << ", ";
			}
			std::cout << std::endl;
		}
	}
}