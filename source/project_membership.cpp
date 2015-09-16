#include <http.h>
#include <project_membership.hpp>

namespace redmine {
project_membership::project_membership() : id(), project(), user(), roles() {}

result project_membership::init(const json::object &object) {
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
result project_memberships(
    const std::string &project, config &config, options options,
    std::vector<project_membership> &project_memberships) {
  std::string body;
  CHECK_RETURN(http::get("/projects/" + project + "/memberships.json", config,
                         options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(has<DEBUG>(options), printf("%s\n", json::write(Root, "  ").c_str()));

  auto Memberships = Root.object().get("memberships");
  CHECK_JSON_PTR(Memberships, json::TYPE_ARRAY);

  for (auto &Membership : Memberships->array()) {
    CHECK_JSON_TYPE(Membership, json::TYPE_OBJECT);

    redmine::project_membership project_membership;
    CHECK_RETURN(project_membership.init(Membership.object()));

    project_memberships.push_back(project_membership);
  }

  return SUCCESS;
}
}
}
