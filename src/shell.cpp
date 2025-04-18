#include <string>
#include <fstream>
#include <exception>
#include <sstream>
#include <functional>

#include "shell.hpp"

std::string exec(const char* cmd) {
    std::ostringstream output;

    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        output << buffer;
    }

    pclose(pipe);
    return output.str();
}

void execCallBack(const char* cmd,std::function<void(const char*)> callback,const std::function<void(int)> exitcb){
    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        callback(buffer);
    }
    

    exitcb(pclose(pipe));
}

std::string cat(const char* path){
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error(std::string("Could not open file ")+path);
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}