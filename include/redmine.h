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

#ifndef REDMINE_H
#define REDMINE_H

#include <defines.h>
#include <error.h>

#include <json/json.hpp>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>

namespace redmine {
/// @brief Enumeration of all possible result codes.
enum result {
  SUCCESS,
  FAILURE,
  UNSUPPORTED,
  ACTION_REQUIRED,
  INVALID_ARGUMENT,
  INVALID_CONFIG,
};

/// @brief Object encapsulating all command line options.
struct options {
  /// @brief Default constructor.
  options() : verbose(), debug(), debug_http() {}

  /// @breif Option to display help output.
  bool help;
  /// @brief Option to enable verbose output.
  bool verbose;
  /// @brief Option to enable debug output.
  bool debug;
  /// @brief Option to enable http debug option.
  ///
  /// Enabling the http debug option is not advised unless ther are issues with
  /// the http connection because the servers response header will be inserted
  /// into the body of the packet invalidating json data.
  bool debug_http;
};

/// @brief Common pattern used to reference a redmine item.
struct reference {
  /// @brief Initialise from a json::object.
  ///
  /// @param object Object to initialise redmine::reference from.
  ///
  /// @return Returns either redmine::SUCCESS or redmine::FAILURE.
  result init(const json::object &object);

  /// @brief The items unique ID number.
  uint32_t id;
  /// @brief Human readable name of the referenced item.
  std::string name;
};
}  // redmine

#endif
