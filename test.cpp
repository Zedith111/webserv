#include <iostream>
#include <string>
#include <vector>

struct serverConf{
	std::string listen;
	std::string server_name;
};

//return advance, -1 if error
int parse(size_t &current, int &indent, serverConf *currentConf,std::vector<std::string> &tokens);
int	parseBasic(size_t &current, int &indent, serverConf *currentConf,std::vector<std::string> &tokens);
int parseLocation(size_t &current, int &indent, serverConf *currentConf,std::vector<std::string> &tokens);
void printConf(serverConf *currentConf);

int main(){
	std::vector<std::string> tokens = {"server", 
	"{", "listen", "80", ";", 
	"server_name", "localhost", ";", 
	"}"};
	serverConf *currentConf = NULL;
	std::vector<serverConf> allConfs;

	int indent_level = 0;
	for (size_t i = 0;i < tokens.size(); i ++){
		// std::cout << "current token: " << tokens[i] << std::endl;
		if (tokens[i] == "{"){
			indent_level ++;
			continue;
		}
		else if (tokens[i] == "}"){
			indent_level--;
			continue;
		}
		if (tokens[i] == "server"){
			if (currentConf != NULL){
				allConfs.push_back(*currentConf);
				currentConf = NULL;
			}
			else{
				serverConf newConf;
				currentConf = &newConf;
			}
			continue;
		}
		else{
			parse(i, indent_level, currentConf,tokens);
		}
	}
	printConf(currentConf);
}

int parse(size_t &current, int &indent, serverConf *currentConf,std::vector<std::string> &tokens){
	std::cout << "current token: " << tokens[current] << std::endl;
	// if (currentConf == NULL){
	// 	std::cout << "Error. No server block found" << std::endl;
	// 	return (0);
	// }

	// if (tokens[current] == "location"){
	// 	return parseLocation(current, indent, currentConf, tokens);
	// }

	// int keyPresent = 0;
	// std::vector<std::string> keys = {"listen", "server_name"};
	// for (size_t i = 0; i < keys.size(); i++){
	// 	if (tokens[current] == keys[i]){
	// 		keyPresent = 1;
	// 		break;
	// 	}
	// }

	// if (keyPresent == 0){
	// 	std::cout << "Error. Invalid key: " << tokens[current] << std::endl;
	// 	return (0);
	// }
	// else{
	// 	return parseBasic(current, indent, currentConf, tokens);
	// }
	return (1);
}

int	parseBasic(size_t &current, int &indent, serverConf *currentConf,std::vector<std::string> &tokens){
	std::cout << "current indent: " << indent << std::endl;
	if (indent != 1){
		std::cout << "Error. Invalid level of indentation for "  << tokens[current] << std::endl;
		return (0);
	}

	if (tokens[current] == "listen"){
		current++;
		if (tokens[current + 1] != ";"){
			std::cout << "Missing ending semicolon after " << tokens[current] << std::endl;
			return (0);
		}
		currentConf->listen = tokens[current];
		return (1);
	}
	return (0);
}
int parseLocation(size_t &current, int &indent, serverConf *currentConf,std::vector<std::string> &tokens){
	return (1);
}

void printConf(serverConf *currentConf){
	std::cout << "listen: " << currentConf->listen << std::endl;
	std::cout << "server_name: " << currentConf->server_name << std::endl;
}