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
	for (size_t i = 0; i < this->server_confs.size(); i++){
		serverConf *current = this->server_confs[i];
		if (current != NULL){
			std::map<std::string, locationInfo *>::iterator it;
			for (it = current->locations.begin(); it != current->locations.end();) {
    			delete it->second;
   				current->locations.erase(it++);
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
	std::cout << COLOR_GREEN << "Parsing Finish" << COLOR_RESET << std::endl;
	return (1);
}

int	ConfigParser::parseServer(size_t &current, int indent_level, serverConf *current_conf){
	std::string	keys[] = {"listen", "server_name", "root", "error_pages", "Add_handler"};
	const int			key_size = sizeof(keys) / sizeof(keys[0]);
	typedef int (ConfigParser::*func)(size_t &, serverConf *);
	func	key_funcs[key_size] = {&ConfigParser::parseListen, &ConfigParser::parseServerName, &ConfigParser::parseRoot, 
					&ConfigParser::parseErrorPages, &ConfigParser::parseCGI};

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
	new_location->autoindex = 0;
	current += 1;
	std::string route = this->tokens[current];

	std::map<std::string, locationInfo *>::iterator it = currentConf->locations.find(route);
	if (it != currentConf->locations.end()){
		std::cout << COLOR_YELLOW << "Warning. Duplicate route when parsing " << this->tokens[current] 
		<< ". Only first route will be used. "<< COLOR_RESET << std::endl;
		delete new_location;
		while (this->tokens[current] != "}"){
			current += 1;
		}
		return (1);
	}

	current += 1;
	if (this->tokens[current] != "{"){
		std::cout << COLOR_RED << "Error. Missing { after location path" << COLOR_RESET << std::endl;
		return (0);
	}
	current += 1;
	std::string keys[] = {"autoindex", "root", "index", "limit_except", "client_max_body_size", "return"};
	const int key_size = sizeof(keys) / sizeof(keys[0]);
	typedef int (ConfigParser::*func)(size_t &, locationInfo *);
	func	key_funcs[key_size] = {&ConfigParser::parseAutoindex, &ConfigParser::parseLocationRoot, 
	&ConfigParser::parseLocationIndex, &ConfigParser::parseLimitExcept, &ConfigParser::parseMaxBodySize, &ConfigParser::parseRedirection};
	
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
			delete new_location;
			return (0);
		}
		if ((this->*key_funcs[keyword_found])(current, new_location) == 0){
			delete new_location;
			return (0);
		}
		current += 1;
	}
	currentConf->locations[route] = new_location;
	return 1;
}

/**
 * @brief Check and fill out necessary information if the config file does not provide.
 * This includes:
 * 		- Necessary information, including host, port,root, server_name
 * 		- Check the error pages, if one is missing, fill out with default error pages
 * 		- Attach server root to location root
 * 		- Check if has duplicate server block. Server block is considered duplicate if it has same host and port, the 
 * 			first one will be used
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
	validateServer();
	return (1);
}

int	ConfigParser::parseListen(size_t &current, serverConf *current_conf){
	current += 1;
	if (!current_conf->host.empty()){
		std::cout << "Current: " << this->tokens[current] << std::endl;
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
	//Check for key present here
	current_conf->error_pages[error_code] = this->tokens[current];
	current +=1;
	return (checkEnding(current));
}

int ConfigParser::parseCGI(size_t &current, serverConf *current_conf){
	current += 1;
	std::string cgi_path = this->tokens[current];
	current += 1;
	while (this->tokens[current] != ";"){
		if (this->tokens[current][0] != '.'){
			std::cout << COLOR_RED << "Error. Invalid extension: " << this->tokens[current] 
			<< ". Extension must start with '.'"<< COLOR_RESET << std::endl;
			return (0);
		}
		current_conf->cgi.insert(std::pair<std::string, std::string>(this->tokens[current], cgi_path));
		current += 1;
	}
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

int	ConfigParser::parseMaxBodySize(size_t &current, locationInfo *current_loc){
	current += 1;
	std::istringstream iss(this->tokens[current]);
	int max_body_size;
	if (!(iss >> max_body_size)){
		std::cout << COLOR_RED << "Error. Invalid max body size: " << this->tokens[current] << COLOR_RESET << std::endl;
		return (0);
	}
	current_loc->max_body_size = max_body_size;
	current += 1;
	return (checkEnding(current));
}

int	ConfigParser::parseRedirection(size_t &current, locationInfo *current_loc){
	current += 1;
	current_loc->redirect_address = this->tokens[current];
	current += 1;
	return (checkEnding(current));
}

int ConfigParser::initDefaultErrorpages(){
	this->default_error_pages[403] = "./default_error_pages/403.html";
	this->default_error_pages[404] = "./default_error_pages/404.html";
	this->default_error_pages[405] = "./default_error_pages/405.html";
	this->default_error_pages[500] = "./default_error_pages/500.html";
	this->default_error_pages[501] = "./default_error_pages/501.html";

	std::map<int, std::string>::iterator it;
	for (it = this->default_error_pages.begin(); it != this->default_error_pages.end(); it++){
		if (checkIsDirectory(it->second) != 0){
			std::cout << COLOR_RED << "Error. Invalid default error page path: " << it->second << COLOR_RESET << std::endl;
			return (0);
		}
		std::ifstream file(it->second.c_str());
		if (!file.is_open()){
			std::cout << COLOR_RED << "Error. Cannot open default error page: " << it->second << COLOR_RESET << std::endl;
			return (0);
		}
		file.close();
	}
	return (1);
}

void ConfigParser::addErrorpages(serverConf *current_conf){
	int all_codes[] = {403, 404, 405};
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
		else{
			it->second->root = current_conf->root;
		}
	}
	return (1);
}

void	ConfigParser::validateServer(){
	for (size_t i = 0; i < this->server_confs.size() - 1; i++){
		size_t j = i + 1;
		if (this->server_confs[i]->host == this->server_confs[j]->host){
			for (size_t k = 0; k < this->server_confs[i]->port_number.size(); k++){
				for (size_t l = 0; l < this->server_confs[j]->port_number.size(); l++){
					if (this->server_confs[i]->port_number[k] == this->server_confs[j]->port_number[l]){
						std::cout << COLOR_YELLOW << "Warning. Dupilcate port: " << this->server_confs[i]->port_number[k] <<
						". Only the first one will be used" << COLOR_RESET << std::endl;
						this->server_confs[j]->port_number.erase(this->server_confs[j]->port_number.begin() + l);
					}
				}
			}
		}
	}
}

int ConfigParser::checkEnding(size_t &current){
	if (this->tokens[current] != ";"){
		std::cout << COLOR_RED << "Error: missing ending character ; after " << this->tokens[current - 1] << COLOR_RESET << std::endl;
		return (0);
	}
	return (1); 
}

std::vector<serverConf *> ConfigParser::getConfigs(){
	return (this->server_confs);
}

size_t ConfigParser::getServerCount(){
	return (this->server_confs.size());
}

void	ConfigParser::printConfs(){
	for (size_t i = 0; i < this->server_confs.size(); i ++){
		serverConf current = *(this->server_confs[i]);
		std::cout << current << std::endl;
	}
}