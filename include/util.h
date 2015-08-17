#ifndef UTIL_H
#define UTIL_H

#include <redmine.h>

#include <string>

namespace util {
std::string getcwd();

result_t rm(const std::string &filename);
}

#endif
