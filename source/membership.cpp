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
#include <membership.h>

redmine::membership::membership() : id(), project(), user(), roles() {}

redmine::result redmine::membership::init(const json::object &object) {
  auto Id = object.get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
  id = Id->number<uint32_t>();

  auto Project = object.get("project");
  CHECK_JSON_PTR(Project, json::TYPE_OBJECT);
  CHECK_RETURN(project.init(Project->object()));

  auto User = object.get("user");
  CHECK_JSON_PTR(User, json::TYPE_OBJECT);
  CHECK_RETURN(user.init(User->object()));

  auto Roles = object.get("roles");
  CHECK_JSON_PTR(Roles, json::TYPE_ARRAY);
  for (auto &Role : Roles->array()) {
    CHECK_JSON_TYPE(Role, json::TYPE_OBJECT);

    reference role;
    CHECK_RETURN(role.init(Role.object()));

    roles.push_back(role);
  }

  return SUCCESS;
}

redmine::result redmine::query::memberships(
    const std::string &project, config &config, redmine::options &options,
    std::vector<membership> &memberships) {
  std::string body;
  CHECK_RETURN(http::get(
      "/projects/" + project + "/memberships.json?offset=0&limit=1000000",
      config, options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(options.debug, printf("%s\n", json::write(Root, "  ").c_str()));

  auto Limit = Root.object().get("limit");
  CHECK_JSON_PTR(Limit, json::TYPE_NUMBER);
  uint32_t limit = Limit->number<uint32_t>();

  auto TotalCount = Root.object().get("total_count");
  CHECK_JSON_PTR(TotalCount, json::TYPE_NUMBER);
  uint32_t total_count = TotalCount->number<uint32_t>();

  json::value *Memberships = Root.object().get("memberships");
  CHECK_JSON_PTR(Memberships, json::TYPE_ARRAY);

  for (auto &Membership : Memberships->array()) {
    CHECK_JSON_TYPE(Membership, json::TYPE_OBJECT);

    redmine::membership membership;
    CHECK_RETURN(membership.init(Membership.object()));

    memberships.push_back(membership);
  }

  return SUCCESS;
}
