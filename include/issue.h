#ifndef ISSUE_H
#define ISSUE_H

#include <config.h>
#include <redmine.h>

#include <json/json.hpp>

#include <vector>
#include <string>

struct issue_t {
  issue_t()
      : id(),
        subject(),
        description(),
        start_date(),
        due_date(),
        created_on(),
        updated_on(),
        done_ratio(),
        estimated_hours(),
        project(),
        tracker(),
        status(),
        priority(),
        author(),
        category() {}

  std::string id;
  std::string subject;
  std::string description;
  std::string start_date;
  std::string due_date;
  std::string created_on;
  std::string updated_on;

  uint32_t done_ratio;
  uint32_t estimated_hours;

  reference_t project;
  reference_t tracker;
  reference_t status;
  reference_t priority;
  reference_t author;
  reference_t assigned_to;
  reference_t category;

  // TODO: Custom fields
};

namespace action {
result_t issue(int argc, char **argv, options_t options);

result_t issue_list(int argc, char **argv, options_t options);

result_t issue_new(int argc, char **argv, options_t options);

result_t issue_show(int argc, char **argv, options_t options);

result_t issue_edit(int argc, char **argv, options_t options);
}

result_t issue_serialize(const issue_t &issue, json::object &out);

result_t issue_deserialize(const json::object &issue, issue_t &out);

result_t issue_list_fetch(std::string &filter, config_t &config,
                          options_t options, std::vector<issue_t> &out);

#endif
