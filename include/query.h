#ifndef REDMINE_QUERY_H
#define REDMINE_QUERY_H

#include <config.h>

struct enumeration_t {
  enumeration_t() : name(), id(0), is_default(false) {}

  std::string name;
  uint32_t id;
  bool is_default;
};

namespace query {
result_t trackers(config_t &config, options_t options,
                  std::vector<reference_t> &trackers);

result_t enumerations(const std::string &enum_name, config_t &config,
                      options_t options, std::vector<enumeration_t> &enums);

result_t time_entry_activities(
    config_t &config, options_t options,
    std::vector<enumeration_t> &time_entry_activities);
}

#endif  // REDMINE_QUERY_H
