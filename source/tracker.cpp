#include <http.h>
#include <tracker.hpp>

namespace redmine {
result query::trackers(config &config, options options,
                       std::vector<reference> &trackers) {
  std::string body;
  CHECK_RETURN(http::get("/trackers.json", config, options, body));

  auto root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto Trackers = root.object().get("trackers");
  CHECK_JSON_PTR(Trackers, json::TYPE_ARRAY);

  for (auto &Tracker : Trackers->array()) {
    CHECK_JSON_TYPE(Tracker, json::TYPE_OBJECT);

    reference tracker;
    CHECK_RETURN(tracker.init(Tracker.object()));

    trackers.push_back(tracker);
  }

  return SUCCESS;
}
}
