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

#include <http.h>
#include <version.h>

#include <json/json.hpp>

namespace redmine {
version::version()
    : id(),
      name(),
      description(),
      status(),
      due_date(),
      created_on(),
      updated_on(),
      project() {}

result version::init(const json::object &object) {
  auto Id = object.get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
  id = Id->number<uint32_t>();

  auto Name = object.get("name");
  CHECK_JSON_PTR(Name, json::TYPE_STRING);
  name = Name->string();

  auto Description = object.get("description");
  CHECK_JSON_PTR(Description, json::TYPE_STRING);
  description = Description->string();

  auto Status = object.get("status");
  CHECK_JSON_PTR(Status, json::TYPE_STRING);
  status = Status->string();

  auto DueDate = object.get("due_date");
  if (DueDate) {
    CHECK_JSON_TYPE(*DueDate, json::TYPE_STRING);
    due_date = DueDate->string();
  }

  auto CreateOn = object.get("created_on");
  CHECK_JSON_PTR(CreateOn, json::TYPE_STRING);
  created_on = CreateOn->string();

  auto UpdatedOn = object.get("updated_on");
  CHECK_JSON_PTR(UpdatedOn, json::TYPE_STRING);
  updated_on = UpdatedOn->string();

  auto Project = object.get("project");
  CHECK_RETURN(project.init(Project->object()));

  return SUCCESS;
}

namespace query {
result versions(const std::string &project, redmine::config &config,
                redmine::options &options, std::vector<version> &versions) {
  std::string body;
  CHECK_RETURN(http::get(
      "/projects/" + project + "/versions.json?offset=0&limit=1000000", config,
      options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(options.debug, printf("%s\n", json::write(Root, "  ").c_str()));

  auto Versions = Root.object().get("versions");
  CHECK_JSON_PTR(Versions, json::TYPE_ARRAY);

  for (auto &Version : Versions->array()) {
    CHECK_JSON_TYPE(Version, json::TYPE_OBJECT);

    redmine::version version;
    CHECK_RETURN(version.init(Version.object()));

    versions.push_back(version);
  }

  return SUCCESS;
}
}  // query
}  // redmine
