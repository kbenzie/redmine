#ifndef PROJECT_H
#define PROJECT_H

#include <config.h>
#include <redmine.h>

#include <json/json.hpp>

#include <string>
#include <vector>

struct project_t {
  std::string name;
  uint32_t id;
  std::string identifier;
  std::string description;
  std::string homepage;
  std::string created_on;  // TODO: Convert to date type
  std::string updated_on;  // TODO: Convert to date type
  reference_t parent;
};

namespace action {
result_t project(int argc, char **argv, options_t options);

result_t project_list(int argc, char **argv, options_t options);

result_t project_new(int argc, char **argv, options_t options);

result_t project_show(int argc, char **argv, options_t options);
}

namespace project {
result_t serialize(const project_t &project, json::object &out);

result_t deserialize(const json::object &project, project_t &out);

project_t *find(std::vector<project_t> &projects, const char *pattern);
}

namespace query {
result_t projects(config_t &config, options_t options,
                  std::vector<project_t> &out);
}

#endif
