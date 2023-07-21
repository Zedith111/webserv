#include <iostream>
#include <string>
#include <vector>
#include <sstream>

enum TEST{
    con_a = 0,
    con_b,
    con_c,
    conc_count
};

std::string con_a_fuc(int a){
    std::cout << a << std::endl;
    return ("A");
}

std::string con_b_fuc(int a){
    std::cout << a << std::endl;
    return ("B");
}

std::string con_c_fuc(int a){
    std::cout << a << std::endl;
    return ("C");
}

int main(){
    typedef std::string (*fptr)(int);
    fptr arr[conc_count] = {con_a_fuc, con_b_fuc, con_c_fuc};

    TEST test = con_b;
    std::cout << arr[test](42) << std::endl;
}
