#include <http.h>
#include <version.hpp>

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
  CHECK_JSON_PTR(DueDate, json::TYPE_STRING);
  due_date = DueDate->string();

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
result versions(const std::string &project, config &config, options options,
                std::vector<version> &versions) {
  std::string body;
  CHECK_RETURN(http::get("/projects/" + project + "/versions.json", config,
                         options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(has<DEBUG>(options), printf("%s\n", json::write(Root, "  ").c_str()));

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
}
}
