#include <iostream>
#include <string>
#include <vector>
#include <sstream>

int main(){
	std::string test= "Hello World";
    size_t pos = test.find("World");

    std::string res = test.substr(pos + 5);

    std::cout << pos << std::endl;
    std::cout << res << std::endl;
    std::cout << res.empty() << std::endl;
}
