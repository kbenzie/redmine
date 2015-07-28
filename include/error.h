#ifndef ERROR_H
#define ERROR_H

enum result {
  SUCCESS,
  FAILURE,
  UNSUPPORTED,
  INVALID_ARGUMENT,
  INVALID_POINTER,
  INVALID_CONFIG,
};

#define CHECK(CONDITION, ACTION)                                               \
  if (CONDITION) {                                                             \
    ACTION;                                                                    \
  }

#endif
