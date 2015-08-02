#ifndef PROJECT_H
#define PROJECT_H

#include <config.h>
#include <redmine.h>

#include <string>
#include <vector>

struct project_t {
  std::string name;
  int id;
  std::string identifier;
  std::string description;
  std::string created_on;  // TODO: Convert to date type
  std::string updated_on;  // TODO: Convert to date type
  int status;
  struct {
    std::string name;
    int id;
  } parent;
};

namespace action {
result_t project(int argc, char **argv, options_t options);

result_t project_show(int argc, char **argv, options_t options);

result_t project_list(int argc, char **argv, options_t options);
}

result_t project_list_fetch(config_t &config, options_t options,
                            std::vector<project_t> &out);

#endif
