// Copyright (C) 2015 Kenneth Benzie
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

#include <json/json.hpp>

#include <iomanip>
#include <sstream>

struct indent_t {
  indent_t(const char *str) : str(str), count(0) {}

  void operator++(int) { count++; }
  void operator--(int) { count--; }

  const char *str;
  size_t count;
};

struct position_t {
  position_t() : line(1), column(1), index(0) {}

  position_t &operator++() {
    column++;
    index++;
    return *this;
  }
  position_t &operator++(int) {
    column++;
    index++;
    return *this;
  }
  position_t &operator+=(size_t count) {
    column += count;
    index += count;
    return *this;
  }

  size_t line;
  size_t column;
  size_t index;
};

struct diagnostic_t {
  diagnostic_t() : error(nullptr) {}

  operator bool() { return error; }

  const char *error;
};

bool consume_whitespace(const char *str, position_t &pos) {
  while (true) {
    switch (str[pos.index]) {
      case ' ':
      case '\t':
        pos++;
        break;
      case '\n':
        pos.line++;
        pos.column = 1;
        pos.index++;
        break;
      case '\0':
        return false;
      default:
        return true;
    }
  }
}

std::string read_string(const char *str, position_t &pos, diagnostic_t &diag);

json::value read_value(const char *str, position_t &pos, diagnostic_t &diag);

json::object read_object(const char *str, position_t &pos, diagnostic_t &diag) {
  pos++;
  json::object object;
  while (true) {
    if (!consume_whitespace(str, pos)) {
      diag.error =
          "No closing object terminator '}' found before end of stream.";
      return {};
    }

    switch (str[pos.index]) {
      case ',': {
        pos++;
      } break;
      case '"': {
        auto key = read_string(str, pos, diag);
        if (diag) {
          // NOTE: Piggy back on diagnostic set by read_string
          return {};
        }
        if (!consume_whitespace(str, pos)) {
          diag.error = "Expected ':' before reaching end of stream";
          return {};
        }
        if (':' != str[pos.index]) {
          diag.error =
              "Unexpected character, expected ':' key value separator.";
          return {};
        }
        pos++;
        if (!consume_whitespace(str, pos)) {
          diag.error = "Expected data value before reaching end of stream.";
          return {};
        }
        auto value = read_value(str, pos, diag);
        if (diag) {
          // NOTE: Piggy back on diagnostic set by read_value
          return {};
        }
        object.add({key, value});
      } break;
      case '}': {
        pos++;
        return object;
      }
      case '\n': {
        pos.line++;
        pos.column = 1;
        pos.index++;
      } break;
      default: { pos++; } break;
    }
  }

  diag.error = "Unexpected character whilst attempting to read object.";
  return {};
}

json::array read_array(const char *str, position_t &pos, diagnostic_t &diag) {
  pos++;
  json::array array;
  while (true) {
    if (!consume_whitespace(str, pos)) {
      diag.error = "Reached end of stream whilst attempting to read array.";
      return {};
    }

    switch (str[pos.index]) {
      case ',': {
        pos++;
      } break;
      case '{':
      case '[':
      case '-':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '"':
      case 't':
      case 'f':
      case 'n': {
        auto value = read_value(str, pos, diag);
        if (diag) {
          // NOTE: Piggy back on diagnostic set by read_value
          return {};
        }
        array.append(value);
      } break;
      case ']': {
        pos++;
        return array;
      }
      case '\n': {
        pos.line++;
        pos.column = 1;
        pos.index++;
      } break;
      default: { pos++; } break;
    }
  }

  diag.error = "Unexpected character whilst attempting to read array.";
  return {};
}

double read_number(const char *str, position_t &pos, diagnostic_t &diag) {
  const char *start = str + pos.index;
  char *end = nullptr;
  double number = strtod(start, &end);
  pos += end - start;
  return number;
}

std::string read_string(const char *str, position_t &pos, diagnostic_t &diag) {
  pos++;

  const char *start = str + pos.index;
  while (true) {
    switch (str[pos.index]) {
      case '"': {
        auto string = std::string(start, str + pos.index);
        pos++;
        return string;
      }
      case '\\': {
        // TODO: Handle escape codes
      } break;
      case '\0': {
        diag.error = "No closing '\"' string terminator before end of stream.";
        return {};
      }
      case '\n': {
        pos.line++;
        pos.column = 1;
      } break;
      default: {
        // NOTE: Process next character
      } break;
    }
    ++pos;
  }

  diag.error = "String reading is not yet implemented.";
  return {};
}

json::value read_value(const char *str, position_t &pos, diagnostic_t &diag) {
  if (!consume_whitespace(str, pos)) {
    diag.error = "Reached end of stream whilst attempting to read value.";
    return {};
  }

  switch (str[pos.index]) {
    case '{': {
      return json::value(read_object(str, pos, diag));
    }
    case '[': {
      return json::value(read_array(str, pos, diag));
    }
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      return json::value(read_number(str, pos, diag));
    }
    case '"': {
      return json::value(read_string(str, pos, diag));
    }
    case 't': {
      if ('r' == str[pos.index + 1] && 'u' == str[pos.index + 2] &&
          'e' == str[pos.index + 3]) {
        pos += 4;
        return json::value(true);
      }
      diag.error = "Expected boolean literal 'true'.";
      return {};
    }
    case 'f': {
      if ('a' == str[pos.index + 1] && 'l' == str[pos.index + 2] &&
          's' == str[pos.index + 3] && 'e' == str[pos.index + 4]) {
        pos += 5;
        return json::value(false);
      }
      diag.error = "Expected boolean literal 'false'.";
      return {};
    }
    case 'n': {
      if ('u' == str[pos.index + 1] && 'l' == str[pos.index + 2] &&
          'l' == str[pos.index + 3]) {
        pos += 4;
        return json::value();
      }
      diag.error = "Expected literal 'null'.";
      return {};
    }
    case '\0': {
      diag.error = "Reached null terminator whilst attempting to read value.";
      return {};
    }
    default: {
      diag.error = "Unexpected character whilst attempting to read value.";
      return {};
    }
  }

  diag.error = "Reached end of stream whilst attempting to read value.";
  return {};
}

json::value json::read(const std::string &string, bool diag_on) {
  position_t pos;
  diagnostic_t diag;
  json::value value = read_value(string.c_str(), pos, diag);
  if (diag_on && diag) {
    fprintf(stderr, "error: %zu:%zu: %s\n", pos.line, pos.column, diag.error);
  }
  return value;
}

void push(const indent_t &indent, std::stringstream &stream) {
  for (uint32_t i = 0; i < indent.count; ++i) {
    stream << indent.str;
  }
}

void write_value(const json::value &value, indent_t &indent,
                 std::stringstream &stream);

void write_object(const json::object &object, indent_t &indent,
                  std::stringstream &stream) {
  stream << "{";
  indent++;
  bool first = true;
  for (auto &pair : object) {
    if (!first) {
      stream << ",";
    }
    stream << "\n";
    push(indent, stream);
    stream << "\"" << pair.first << "\": ";
    write_value(pair.second, indent, stream);
    first = false;
  }
  indent--;
  stream << "\n";
  push(indent, stream);
  stream << "}";
}

void write_array(const json::array &array, indent_t &indent,
                 std::stringstream &stream) {
  stream << "[";
  indent++;
  bool first = true;
  for (auto &value : array) {
    if (!first) {
      stream << ",";
    }
    stream << "\n";
    push(indent, stream);
    write_value(value, indent, stream);
    first = false;
  }
  indent--;
  stream << "\n";
  push(indent, stream);
  stream << "]";
}

void write_value(const json::value &value, indent_t &indent,
                 std::stringstream &stream) {
  switch (value.type()) {
    case json::TYPE_OBJECT:
      write_object(value.object(), indent, stream);
      break;
    case json::TYPE_ARRAY:
      write_array(value.array(), indent, stream);
      break;
    case json::TYPE_NUMBER:
      stream << std::setprecision(16) << value.number();
      break;
    case json::TYPE_STRING:
      stream << "\"" << value.string() << "\"";
      break;
    case json::TYPE_BOOL:
      stream << (value.boolean() ? "true" : "false");
      break;
    case json::TYPE_NULL:
      stream << "null";
      break;
  }
}

std::string json::write(const json::value &value, const char *tab) {
  indent_t indent(tab);
  std::stringstream stream;
  write_value(value, indent, stream);
  return stream.str();
}
