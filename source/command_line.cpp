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
