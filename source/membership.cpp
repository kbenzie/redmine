#include <http.h>
#include <membership.h>

namespace redmine {
membership::membership() : id(), project(), user(), roles() {}

result membership::init(const json::object &object) {
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

namespace query {
result memberships(const std::string &project, config &config,
                   redmine::options &options,
                   std::vector<membership> &memberships) {
  std::string body;
  CHECK_RETURN(http::get("/projects/" + project + "/memberships.json", config,
                         options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(options.debug, printf("%s\n", json::write(Root, "  ").c_str()));

  auto Limit = Root.object().get("limit");
  CHECK_JSON_PTR(Limit, json::TYPE_NUMBER);
  uint32_t limit = Limit->number<uint32_t>();

  auto TotalCount = Root.object().get("total_count");
  CHECK_JSON_PTR(TotalCount, json::TYPE_NUMBER);
  uint32_t total_count = TotalCount->number<uint32_t>();

  json::value *Memberships = nullptr;
  if (limit < total_count) {
    CHECK_RETURN(http::get("/projects/" + project +
                               "/memberships.json?offset=0&limit=" +
                               std::to_string(total_count),
                           config, options, body));
    auto RootFull = json::read(body, false);
    CHECK_JSON_TYPE(RootFull, json::TYPE_OBJECT);
    CHECK(options.debug,
          printf("second %s\n", json::write(RootFull, "  ").c_str()));
    Memberships = RootFull.object().get("memberships");
  } else {
    Memberships = Root.object().get("memberships");
  }
  CHECK_JSON_PTR(Memberships, json::TYPE_ARRAY);

  for (auto &Membership : Memberships->array()) {
    CHECK_JSON_TYPE(Membership, json::TYPE_OBJECT);

    redmine::membership membership;
    CHECK_RETURN(membership.init(Membership.object()));

    memberships.push_back(membership);
  }

  return SUCCESS;
}
}  // query
}  // redmine
