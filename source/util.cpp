// Copyright (C) 2015 Kenenth Benzie
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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
