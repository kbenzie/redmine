#include <util.h>

#include <cstdio>

#if defined(REDMINE_PLATFORM_LINUX) || defined(REDMINE_PLATFORM_MAC)
#include <unistd.h>
#elif defined(REDMINE_PLATFORM_WINDOWS)
#include <direct.h>
#include <Windows.h>
#endif

namespace redmine {
namespace util {
std::string getcwd() {
  char buffer[4096] = {};
#if defined(REDMINE_PLATFORM_LINUX) || defined(REDMINE_PLATFORM_MAC)
  return ::getcwd(buffer, 4096);
#elif defined(REDMINE_PLATFORM_WINDOWS)
  return _getcwd(buffer, 4096);
#endif
  return std::string(buffer);
}

result rm(const std::string &filename) {
#if defined(REDMINE_PLATFORM_LINUX) || defined(REDMINE_PLATFORM_MAC)
  CHECK(unlink(filename.c_str()),
        fprintf(stderr, "could not delete file: %s\n", filename.c_str());
        return FAILURE);
#elif defined(REDMINE_PLATFORM_WINDOWS)
    CHECK(!DeleteFile(filename.c_str()),
          fprintf(stderr, "could not delete file: %s\n", filename.c_str());
          return FAILURE);
#endif
  return SUCCESS;
}
}
}
