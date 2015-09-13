#include <http.h>
#include <query.h>

result_t query::trackers(config_t &config, options_t options,
                         std::vector<reference_t> &trackers) {
  std::string body;
  CHECK_RETURN(http::get("/trackers.json", config, options, body));

  auto root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto Trackers = root.object().get("trackers");
  CHECK_JSON_PTR(Trackers, json::TYPE_ARRAY);

  for (auto &Tracker : Trackers->array()) {
    CHECK_JSON_TYPE(Tracker, json::TYPE_OBJECT);

    reference_t tracker;
    CHECK_RETURN(reference_deserialize(Tracker.object(), tracker));

    trackers.push_back(tracker);
  }

  return SUCCESS;
}

result_t query::enumerations(const std::string &enum_name, config_t &config,
                             options_t options,
                             std::vector<enumeration_t> &enums) {
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
    enumeration_t enumeration;

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

result_t query::time_entry_activities(
    config_t &config, options_t options,
    std::vector<enumeration_t> &time_entry_activities) {
  return enumerations("time_entry_activities", config, options,
                      time_entry_activities);
}
