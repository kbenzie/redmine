#include <command_line.h>

redmine::cl::args::args(int argc, char **argv) : argc(argc), argv(argv) {}

redmine::cl::args::args(const args &other)
    : argc(other.argc), argv(other.argv) {}

redmine::cl::args &redmine::cl::args::operator++() {
  --argc;
  ++argv;
  return *this;
}

redmine::cl::args &redmine::cl::args::operator++(int) {
  --argc;
  ++argv;
  return *this;
}

redmine::cl::args &redmine::cl::args::operator+=(const int count) {
  argc -= count;
  argv += count;
  return *this;
}

redmine::cl::args &redmine::cl::args::operator=(
    const redmine::cl::args &other) {
  argc = other.argc;
  argv = other.argv;
  return *this;
}

const char *redmine::cl::args::operator[](const int index) {
  return argv[index];
}

const char *redmine::cl::args::front() { return argv[0]; }

const char *redmine::cl::args::back() { return argv[argc - 1]; }

const char *const *redmine::cl::args::begin() { return argv; }

const char *const *redmine::cl::args::end() { return argv + argc; }

int redmine::cl::args::count() { return argc; }
