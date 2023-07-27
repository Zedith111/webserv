#include <iostream>
#include <string>
#include <sys/stat.h>

int main() {
    std::string relative = "/www";
    std::string absolute = "/goinfre/zah/webserv/www";

    std::string path = relative;

    struct stat file_info;
    int res = stat(path.c_str(), &file_info);
    if (res < 0){
        std::string test_path = "." + path;
        res = stat(test_path.c_str(), &file_info);
        if (res < 0){
            std::cout << "Failed" << std::endl;
            return (1);
        }
    }

  if (S_ISDIR(file_info.st_mode)) {
    std::cout << "The request path is a directory." << std::endl;
  } else {
    std::cout << "The request path is a file." << std::endl;
  }

  return 0;
}

