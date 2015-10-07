#include <command_line.h>

#include <iostream>

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

std::string redmine::cl::get_answer_string(const std::string &question) {
  std::cout << question << ": ";
  std::string answer;
  std::getline(std::cin, answer);
  return answer;
}

uint32_t redmine::cl::get_answer_number(const std::string &question) {
  uint32_t answer = 0;
  while (true) {
    std::cout << question << " (number): ";
    std::string str;
    std::getline(std::cin, str);
    char *end = nullptr;
    answer = std::strtoul(str.c_str(), &end, 10);
    if (str.data() + str.size() == end) {
      break;
    }
  }
  return answer;
}

bool redmine::cl::get_answer_bool(const std::string &question) {
  while (true) {
    std::cout << question << " (true|false): ";
    std::string answer;
    std::getline(std::cin, answer);
    if ("true" == answer) {
      return true;
    } else if ("false" == answer) {
      return false;
    }
  }
  return false;
}
