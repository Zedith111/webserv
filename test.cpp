#include <iostream>
#include <string>
#include <vector>

// std::vector<std::string> tokenize(const std::string& input, const std::string& delimiters, const std::string& specialChars) {
//     std::vector<std::string> tokens;
//     std::string token;
//     std::size_t pos = 0;
//     std::size_t start = 0;

//     while ((pos = input.find_first_of(delimiters + specialChars, start)) != std::string::npos) {
//         token = input.substr(start, pos - start);
//         start = pos + 1;
//         tokens.push_back(token);
//         if (specialChars.find(input[pos]) != std::string::npos) {
//             token = input.substr(pos, 1);
//             tokens.push_back(token);
//         }
//     }

//     if (start < input.length()) {
//         token = input.substr(start);
//         tokens.push_back(token);
//     }

//     return tokens;
// }

// int main() {
//     std::string input = "Hello,world!How-are||you?Fine.";
//     std::string delimiters = ",-|!";
//     std::string specialChars = "?";

//     std::vector<std::string> tokens = tokenize(input, delimiters, specialChars);

//     for (const std::string& token : tokens) {
//         std::cout << token << std::endl;
//     }

//     return 0;
// }

class Tokenizer{
    private :
        std::vector<std::string> tokens;
        
}

//Rettun
void tokenize(std::string &str, char *delim, char *special){
    int begin;
    int end;

}

int main(){
    std::string msg = "Hello World!!!     How are you?    Fine.";
    char *delim = " \t\r";
    char *special = "!?";
}
