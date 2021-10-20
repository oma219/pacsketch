/*
 * Name: pacsketch.cpp
 * Description: Main file for pacsketch project.
 * Project: pacsketch project.
 * 
 * Author: Omar Ahmed
 * Date: October 17, 2021
 */

#include <iostream>
#include <fstream>
#include <hash.h>

bool is_file(const char* file_path) {
    /* Checks if the path is a valid file-path */
    std::ifstream test_file(file_path, std::ifstream::in);
    if (test_file.fail()) {return 0;}
    
    test_file.close();
    return 1;   
}

/*
int main(int argc, char** argv){
    std::cout << "helloworld" << std::endl;

    std::cout << MurmurHash3(32) << std::endl;

}
*/