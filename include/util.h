#ifndef UTIL_H
#define UTIL_H

#include <redmine.h>

#include <string>

namespace redmine {
namespace util {
std::string getcwd();

result rm(const std::string &filename);
}
}

#endif
