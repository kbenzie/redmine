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

#ifndef PROJECT_HPP
#define PROJECT_HPP

#include <config.h>
#include <redmine.h>

#include <json/json.hpp>

#include <string>
#include <vector>

namespace redmine {
struct project {
  /// @brief Default constructor.
  project();

  /// @brief Initialise from a json::object.
  ///
  /// @param object Object to initilise redmine::project with.
  ///
  /// @return Returns either redmine::SUCCESS or redmine::FAILURE.
  result init(const json::object &object);

  /// @brief Construct a json::object from this redmine::project.
  ///
  /// @return The constructed json::object.
  json::object jsonify() const;

  /// @brief Equality operator for redmine::project.
  ///
  /// @param other Another redmine::project object.
  ///
  /// @return Returns true if equal, false otherwise.
  bool operator==(const project &other) const;

  /// @brief Equality operator for string.
  ///
  /// @param str String containing either an id, name or identifier.
  ///
  /// @return Returns true if equal, false otherwise.
  bool operator==(const char *str) const;

  uint32_t id;
  std::string name;
  std::string identifier;
  std::string description;
  std::string homepage;
  std::string created_on;
  std::string updated_on;
  reference parent;
};

namespace action {
result project(redmine::cl::args &args, redmine::config &config,
               redmine::options &options);

result project_list(redmine::cl::args &args, redmine::config &config,
                    redmine::options &options);

result project_new(redmine::cl::args &args, redmine::config &config,
                   redmine::options &options);

result project_show(redmine::cl::args &args, redmine::config &config,
                    redmine::options &options);
}

project *find(std::vector<project> &projects, const char *pattern);

namespace query {
result projects(redmine::config &config, redmine::options &options,
                std::vector<redmine::project> &projects);
}
}

#endif
