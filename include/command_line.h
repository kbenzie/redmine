#ifndef REDMINE_COMMAND_LINE_H
#define REDMINE_COMMAND_LINE_H

namespace redmine {
namespace cl {
/// @brief Command line argument wrapper.
///
/// The redmine::cl::args object wraps the command line arguments passed to a
/// C++ ::main function. No copies of the values in @a argv are made and access
/// to all argument values remain unmodified.
class args {
 public:
  /// @brief Construct from command line arguments.
  ///
  /// @param argc Argument count.
  /// @param argv Argument values.
  args(int argc, char **argv);

  /// @brief Copy constructor.
  ///
  /// @param other Object to copy from.
  args(const args &other);

  /// @brief Copy assignment operator.
  ///
  /// @param other Object to copy assign from.
  ///
  /// @return Reference to this object.
  args &operator=(const args &other);

  /// @brief Consume first argument.
  ///
  /// @return Reference to this object.
  args &operator++();

  /// @brief Consume first argument.
  ///
  /// @return Reference to this object.
  args &operator++(int);

  /// @brief Consume first count arguments.
  ///
  /// @param count Number of arguments to consume.
  ///
  /// @return Reference to this object.
  args &operator+=(const int count);

  /// @brief Access argument.
  ///
  /// @param index Argument index.
  ///
  /// @return Argument value.
  const char *operator[](const int index);

  /// @brief Return the first argument.
  ///
  /// @return Argument value.
  const char *front();

  /// @brief Return the last argument.
  ///
  /// @return Argument value.
  const char *back();

  /// @brief Return iterator to beginning.
  ///
  /// @return The first element iterator.
  const char *const *begin();

  /// @brief Return iterator to end.
  ///
  /// @return One past the last element iterator.
  const char *const *end();

  /// @brief Return argument count.
  ///
  /// @return Number of arguments.
  int count();

 private:
  /// @brief Argument count.
  int argc;
  /// @brief Argument value array.
  char **argv;
};
}  // cl
}  // redmine

#endif  // REDMINE_COMMAND_LINE_H
