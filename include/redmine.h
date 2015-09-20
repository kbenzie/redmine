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

/// @brief Enumeration of all possible options.
enum option {
  NONE,
  VERBOSE,
  DEBUG,
  DEBUG_HTTP,
};

/// @brief Common pattern used to reference a redmine projcet, issue, and other
/// items.
struct reference {
  /// @brief Initialise from a json::object.
  ///
  /// @param object Object to initialise json::object from.
  ///
  /// @return Returns either redmine::SUCCESS or redmine::FAILURE.
  result init(const json::object &object);

  /// @brief The items unique ID number.
  uint32_t id;
  /// @brief Human readable name of the referenced item.
  std::string name;
};

/// @brief An options_t is a bitfield of option_t values.
typedef uint32_t options;

/// @brief Check if an option is enabled.
///
/// @tparam option Desired option to check for.
///
/// @return true if enabled, false otherwise.
#define HAS_OPTION(OPTION) (OPTION == (OPTION & options))
}

#endif
