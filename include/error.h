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

#ifndef REDMINE_ERROR_H
#define REDMINE_ERROR_H

#include <cstdio>

#ifndef NDEBUG
#include <cstdlib>
/// @brief Force abort and display message when condition is true.
///
/// @param CONDITION Boolean condition to check.
/// @param MESSAGE Informative message displayed before abort.
#define ASSERT(CONDITION, MESSAGE)                                \
  if (CONDITION) {                                                \
    fprintf(stderr, "%s: %d: %s\n", __FILE__, __LINE__, MESSAGE); \
    abort();                                                      \
  }
/// @brief Used to abort on reaching an unreachable code path.
///
/// @param MESSAGE An informative message why the UNREACHABLE occured.
#define UNREACHABLE(MESSAGE)                                      \
  {                                                               \
    fprintf(stderr, "%s: %d: %s\n", __FILE__, __LINE__, MESSAGE); \
    abort();                                                      \
  }
#else
#define ASSERT(CONDITION, MESSAGE)
#define UNREACHABLE(MESSAGE)
#endif

#ifdef REDMINE_DEBUG
#define DEBUG_MSG(FORMAT, ...) \
  fprintf(stderr, "%s: %s: " FORMAT, __FILE__, __LINE__, ##__VA_ARGS__)
#define ERROR_MSG(FORMAT, ...) \
  fprintf(stderr, "%s: %d: " FORMAT, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define DEBUG_MSG(FORMAT, ...)
#define ERROR_MSG(FORMAT, ...) fprintf(stderr, FORMAT, ##__VA_ARGS__)
#endif

/// @brief Check the condition and perform the action.
///
/// @param CONDITION Boolean condition to check.
/// @param ACTION Action to perform if check is true.
#define CHECK(CONDITION, ACTION) \
  if (CONDITION) {               \
    DEBUG_MSG("check failed\n"); \
    ACTION;                      \
  }

/// @brief Evaluate expression and return result if true.
///
/// @param EXPRESSION Expression to evaluate.
///
/// @return Result of EXPRESSION.
#define CHECK_RETURN(EXPRESSION)                       \
  if (redmine::result result = (EXPRESSION)) {         \
    DEBUG_MSG("%s\n", redmine::result_string(result)); \
    return result;                                     \
  }

/// @brief Check the condition and perform the action.
///
/// @param CONDITION Boolean condition to check.
/// @param ACTION Action to perform if check is true.
#define CHECK_MSG(CONDITION, MESSAGE, ACTION) \
  if (CONDITION) {                            \
    ERROR_MSG("%s\n", MESSAGE);               \
    ACTION;                                   \
  }

/// @brief Check json is not null & is of given type.
///
/// @param REFERENCE Pointer to a json value type.
/// @param TYPE Enumberation of json type.
///
/// @return FAILURE if check is true.
#define CHECK_JSON_TYPE(REFERENCE, TYPE)    \
  if (TYPE != (REFERENCE).type()) {         \
    DEBUG_MSG("json is not a " #TYPE "\n"); \
    return FAILURE;                         \
  }

/// @brief Check json is not null & is of given type.
///
/// @param POINTER Pointer to a json value type.
/// @param TYPE Enumberation of json type.
///
/// @return FAILURE if check is true.
#define CHECK_JSON_PTR(POINTER, TYPE)                \
  if (!POINTER) {                                    \
    DEBUG_MSG("json is null\n", __FILE__, __LINE__); \
    return FAILURE;                                  \
  }                                                  \
  CHECK_JSON_TYPE(*POINTER, TYPE)

#endif  // REDMINE_ERROR_H
