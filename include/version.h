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

#ifndef REDMINE_VERSION_H
#define REDMINE_VERSION_H

#include <redmine.h>
#include <config.h>

namespace redmine {
struct version {
  version();

  result init(const json::object &object);

  uint32_t id;
  std::string name;
  std::string description;
  std::string status;
  std::string due_date;
  std::string created_on;
  std::string updated_on;
  reference project;
};

namespace query {
result versions(const std::string &project, redmine::config &config,
                redmine::options &options, std::vector<version> &versions);
}
}

#endif
