#ifndef REDMINE_H
#define REDMINE_H

#include <redmine_config.h>

#include <cstddef>
#include <cstdint>

/// @brief Enumeration of all possible result codes.
enum result_t {
  SUCCESS,
  FAILURE,
  UNSUPPORTED,
  ACTION_REQUIRED,
  INVALID_ARGUMENT,
  INVALID_CONFIG,
};

/// @brief Enumeration of all possible options.
enum option_t {
  NONE,
  VERBOSE,
  DEBUG,
  DEBUG_HTTP,
};

/// @brief An options_t is a bitfield of option_t values.
typedef uint32_t options_t;

/// @brief Check if an option is enabled.
///
/// @tparam option Desired option to check for.
/// @param options Bitfield of enabled options.
///
/// @return true if enabled, false otherwise.
template <option_t option>
bool has(options_t options) {
  return option == (option & options);
}

/// @brief Force abort and display message when condition is true.
///
/// @param CONDITION Boolean condition to check.
/// @param MESSAGE Informative message displayed before abort.
#define ASSERT(CONDITION, MESSAGE)                                \
  if (CONDITION) {                                                \
    fprintf(stderr, "%s: %d: %s\n", __FILE__, __LINE__, MESSAGE); \
    abort();                                                      \
  }

/// @brief Check the condition and perform the action.
///
/// @param CONDITION Boolean condition to check.
/// @param ACTION Action to perform if check is true.
#define CHECK(CONDITION, ACTION) \
  if (CONDITION) {               \
    ACTION;                      \
  }

/// @brief Evaluate expression and return result if true.
///
/// @param EXPRESSION Expression to evaluate.
#define CHECK_RETURN(EXPRESSION)        \
  if (result_t result = (EXPRESSION)) { \
    return result;                      \
  }

#endif
