#include <enumeration.hpp>
#include <http.h>

#include <json/json.hpp>

namespace redmine {
static result query_enumerations(const std::string &enum_name, config &config,
                                 options options,
                                 std::vector<redmine::enumeration> &enums) {
  std::string body;
  CHECK_RETURN(
      http::get("/enumerations/" + enum_name + ".json", config, options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(has<DEBUG>(options), printf("%s\n", json::write(Root, "  ").c_str()));

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
      CHECK_JSON_TYPE((*is_default), json::TYPE_BOOL);
      enumeration.is_default = is_default->boolean();
    }

    enums.push_back(enumeration);
  }

  return SUCCESS;
}

result query::issue_priorities(config &config, options options,
                               std::vector<enumeration> &priorities) {
  return query_enumerations("issue_priorities", config, options, priorities);
}

result query::time_entry_activities(
    config &config, options options,
    std::vector<enumeration> &time_entry_activities) {
  return query_enumerations("time_entry_activities", config, options,
                            time_entry_activities);
}
}
