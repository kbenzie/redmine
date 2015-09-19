#ifndef REDMINE_VERSION_H
#define REDMINE_VERSION_H

#include <redmine.h>
#include <config.h>

namespace redmine {
struct version {
  version();

  result init(const json::object &object);

  uint32_t id;
  std::string name;
  std::string description;
  std::string status;
  std::string due_date;
  std::string created_on;
  std::string updated_on;
  reference project;
};

namespace query {
result versions(const std::string &project, config &config, options options,
                std::vector<version> &versions);
}
}

#endif
