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

#include <enumeration.h>
#include <http.h>

#include <json/json.hpp>

namespace redmine {
static result query_enumerations(const std::string &enum_name, config &config,
                                 redmine::options &options,
                                 std::vector<redmine::enumeration> &enums) {
  std::string body;
  CHECK_RETURN(
      http::get("/enumerations/" + enum_name + ".json", config, options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(options.debug, printf("%s\n", json::write(Root, "  ").c_str()));

  auto Enums = Root.object().get(enum_name);
  CHECK_JSON_PTR(Enums, json::TYPE_ARRAY);

  for (auto &Enum : Enums->array()) {
    CHECK_JSON_TYPE(Enum, json::TYPE_OBJECT);
    redmine::enumeration enumeration;

    auto name = Enum.object().get("name");
    CHECK_JSON_PTR(name, json::TYPE_STRING);
    enumeration.name = name->string();

    auto id = Enum.object().get("id");
    CHECK_JSON_PTR(id, json::TYPE_NUMBER);
    enumeration.id = id->number<uint32_t>();

    auto is_default = Enum.object().get("is_default");
    if (is_default) {
      CHECK_JSON_TYPE(*is_default, json::TYPE_BOOL);
      enumeration.is_default = is_default->boolean();
    }

    enums.push_back(enumeration);
  }

  return SUCCESS;
}

result query::issue_priorities(redmine::config &config,
                               redmine::options &options,
                               std::vector<enumeration> &priorities) {
  return query_enumerations("issue_priorities", config, options, priorities);
}

result query::time_entry_activities(
    redmine::config &config, redmine::options &options,
    std::vector<enumeration> &time_entry_activities) {
  return query_enumerations("time_entry_activities", config, options,
                            time_entry_activities);
}
}  // redmine
