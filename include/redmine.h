#ifndef REDMINE_H
#define REDMINE_H

#include <defines.h>

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>

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

/// @brief Common pattern used to reference a redmine projcet, issue, and other
/// items.
struct reference_t {
  /// @brief Human readable name of the referenced item.
  std::string name;
  /// @brief The items unique ID number.
  uint32_t id;
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

#ifndef REDMINE_DEBUG
/// @brief Check the condition and perform the action.
///
/// @param CONDITION Boolean condition to check.
/// @param ACTION Action to perform if check is true.
#define CHECK(CONDITION, ACTION) \
  if (CONDITION) {               \
    ACTION;                      \
  }
#else
#define CHECK(CONDITION, ACTION)                     \
  if (CONDITION) {                                   \
    fprintf(stderr, "%s: %d: ", __FILE__, __LINE__); \
    ACTION;                                          \
  }
#endif

#ifndef REDMINE_DEBUG
/// @brief Evaluate expression and return result if true.
///
/// @param EXPRESSION Expression to evaluate.
#define CHECK_RETURN(EXPRESSION)        \
  if (result_t result = (EXPRESSION)) { \
    return result;                      \
  }
#else
const char *result_string(result_t result);
#define CHECK_RETURN(EXPRESSION)                        \
  if (result_t result = (EXPRESSION)) {                 \
    fprintf(stderr, "%s: %d: %s\n", __FILE__, __LINE__, \
            result_string(result));                     \
    return result;                                      \
  }
#endif

#ifndef REDMINE_DEBUG
/// @brief Check json is not null & is of given type.
///
/// @param REFERENCE Pointer to a json value type.
/// @param TYPE Enumberation of json type.
///
/// @return FAILURE if check is true.
#define CHECK_JSON_TYPE(REFERENCE, TYPE)                                      \
  if (TYPE != REFERENCE.type()) {                                             \
    return FAILURE;                                                           \
  }
#else
#define CHECK_JSON_TYPE(REFERENCE, TYPE)                                      \
  if (TYPE != REFERENCE.type()) {                                             \
    fprintf(stderr, "%s: %d: json is not a %s\n", __FILE__, __LINE__, #TYPE); \
    return FAILURE;                                                           \
  }
#endif

#ifndef REDMINE_DEBUG
/// @brief Check json is not null & is of given type.
///
/// @param POINTER Pointer to a json value type.
/// @param TYPE Enumberation of json type.
///
/// @return FAILURE if check is true.
#define CHECK_JSON_PTR(POINTER, TYPE)                              \
  if (!POINTER) {                                                  \
    return FAILURE;                                                \
  }                                                                \
  CHECK_JSON_TYPE((*POINTER), TYPE)
#else
#define CHECK_JSON_PTR(POINTER, TYPE)                              \
  if (!POINTER) {                                                  \
    fprintf(stderr, "%s: %d: json is null\n", __FILE__, __LINE__); \
    return FAILURE;                                                \
  }                                                                \
  CHECK_JSON_TYPE((*POINTER), TYPE)
#endif

#endif
