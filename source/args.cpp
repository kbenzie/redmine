#include <args.h>

redmine::args::args(int argc, char **argv) : argc(argc), argv(argv) {}

redmine::args::args(const args &other) : argc(other.argc), argv(other.argv) {}

redmine::args &redmine::args::operator++() {
  --argc;
  ++argv;
  return *this;
}

redmine::args &redmine::args::operator++(int) {
  --argc;
  ++argv;
  return *this;
}

redmine::args &redmine::args::operator+=(const int count) {
  argc -= count;
  argv += count;
  return *this;
}

redmine::args &redmine::args::operator=(const redmine::args &other) {
  argc = other.argc;
  argv = other.argv;
  return *this;
}

const char *redmine::args::operator[](const int index) { return argv[index]; }

const char *redmine::args::front() { return argv[0]; }

const char *redmine::args::back() { return argv[argc - 1]; }

const char *const *redmine::args::begin() { return argv; }

const char *const *redmine::args::end() { return argv + argc; }

int redmine::args::count() { return argc; }
