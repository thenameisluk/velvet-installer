#pragma once
#include <string>
#include <functional>

std::string exec(const char* cmd);
void execCallBack(const char* cmd,std::function<void(const char*)> callback,const std::function<void(int)> exitcb = [](int exitcode){});
std::string cat(const char* path);