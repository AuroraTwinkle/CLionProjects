//
// Created by aurora on 19-5-12.
//

#include "MyLog.h"

void MyLog::operator()(const std::string &info) {
    std::cout << info << std::endl;
}
