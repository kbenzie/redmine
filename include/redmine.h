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
