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

#include <cmath>

const char *typeStr(json::type type) {
  switch (type) {
    case json::TYPE_OBJECT:
      return "Object";
    case json::TYPE_ARRAY:
      return "Array";
    case json::TYPE_NUMBER:
      return "Number";
    case json::TYPE_STRING:
      return "String";
    case json::TYPE_BOOL:
      return "Bool";
    case json::TYPE_NULL:
      return "Null";
    default:
      return nullptr;
  }
}

int main(int argc, char **argv) {
  json::value value;
  printf("%s\n", typeStr(value.type()));

  value = json::value(42);
  printf("%s\n", typeStr(value.type()));

  value = json::value("String");
  printf("%s\n", typeStr(value.type()));

  json::object object;
  value = json::value(object);
  printf("%s\n", typeStr(value.type()));

  json::array array;
  value = json::value(array);
  printf("%s\n", typeStr(value.type()));

  value = json::value(true);
  printf("%s\n", typeStr(value.type()));

  value = json::value();
  printf("%s\n", typeStr(value.type()));

  value = json::value{
    json::object{
      {"key", json::value(42)},
      {"string", json::value("value")},
    }
  };
  printf("%s\n", typeStr(value.type()));

  object = value.object();
  printf("%s\n", typeStr(object.get("key")->type()));
  printf("Value: %f\n", object.get("key")->number());
  printf("%s\n", typeStr(object.get("string")->type()));
  printf("Value: %s\n", object.get("string")->string().c_str());

  value = json::value{
    json::array{
      json::value(42),
      json::value("string"),
      json::value(false)
    }
  };

  array = value.array();
  for (auto &value : array) {
    printf("%s\n", typeStr(value.type()));
    switch (value.type()) {
      case json::TYPE_NUMBER:
        printf("Value: %f\n", value.number());
        break;
      case json::TYPE_STRING:
        printf("Value: %s\n", value.string().c_str());
        break;
      case json::TYPE_BOOL:
        printf("Value: %s\n", (value.boolean()) ? "true" : "false");
        break;
      default:
        break;
    }
  }

  value = json::value{
    json::object{
      {"key", json::value(42)},
      {"string", json::value("value")},
      {"array", json::value(json::array{
        json::value(4 * atan(1)),
        json::value("string"),
        json::value(true),
        json::value(),
      })}
    }
  };

  std::string json = json::write(value, "  ");
  printf("%s\n", json.c_str());

  value = json::read(json);

  printf("%s\n", typeStr(value.type()));
  object = value.object();

  for (auto &value : object) {
    printf("\"%s\": %s\n", value.first.c_str(), typeStr(value.second.type()));
  }

  array = object.get("array")->array();
  for (auto &value : array) {
    switch (value.type()) {
      case json::TYPE_OBJECT:
      case json::TYPE_ARRAY:
        break;
      case json::TYPE_NUMBER:
        printf("%f\n", value.number());
        break;
      case json::TYPE_STRING:
        printf("\"%s\"\n", value.string().c_str());
        break;
      case json::TYPE_BOOL:
        printf("%s\n", value.boolean() ? "true" : "false");
        break;
      case json::TYPE_NULL:
        printf("null\n");
        break;
    }
  }

  json::array friends = json::array{
    json::value(json::object{
      {"id", json::value(0)},
      {"name", json::value("Ayala Benjamin")},
    }),
    json::value(json::object{
      {"id", json::value(1)},
      {"name", json::value("Cummings Lara")},
    }),
    json::value(json::object{
      {"id", json::value(2)},
      {"name", json::value("Terri Salas")},
    }),
  };
  printf("%s\n", json::write(json::value(friends), "  ").c_str());

  json::array control_characters{json::value{"\""}, json::value{"\\"},
                                 json::value{"/"},  json::value{"\f"},
                                 json::value{"\n"}, json::value{"\r"},
                                 json::value{"\t"}, json::value{"\u0117"}};
  std::string control_characters_string = json::write(control_characters, "  ");
  printf("%s\n", control_characters_string.c_str());
  json::value control_characters_value = json::read(control_characters_string);

  return 0;
}
