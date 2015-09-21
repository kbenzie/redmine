#ifndef ISSUE_H
#define ISSUE_H

#include <args.h>
#include <config.h>
#include <redmine.h>

#include <json/json.hpp>

#include <vector>
#include <string>

namespace redmine {
struct issue {
  /// @brief Default constructor.
  issue();

  /// @brief Initialise from json::object.
  ///
  /// @param object Object to initilise redmine::issue from.
  ///
  /// @return Returns either redmine::SUCCESS or redmine::FAILURE.
  result init(const json::object &object);

  /// @brief Construct a json::object from this redmine::issue.
  ///
  /// @return The constructed json::object.
  json::object jsonify() const;

  uint32_t id;
  std::string subject;
  std::string description;
  std::string start_date;
  std::string due_date;
  std::string created_on;
  std::string updated_on;

  uint32_t done_ratio;
  uint32_t estimated_hours;

  reference project;
  reference tracker;
  reference status;
  reference priority;
  reference author;
  reference assigned_to;
  reference category;

  // TODO: Custom fields?
};

struct issue_status {
  uint32_t id;
  std::string name;
  bool is_default;
  bool is_closed;
};

struct issue_category {
  uint32_t id;
  std::string name;
  reference project;
  reference assigned_to;
};

namespace action {
result issue(redmine::args args, redmine::config &config,
             redmine::options &options);
result issue_list(redmine::args args, redmine::config &config,
                  redmine::options &options);
result issue_new(redmine::args args, redmine::config &config,
                 redmine::options &options);
result issue_show(redmine::args args, redmine::config &config,
                  redmine::options &options);
result issue_update(redmine::args args, redmine::config &config,
                    redmine::options options);
}

namespace query {
result issues(std::string &filter, redmine::config &config,
              redmine::options options, std::vector<issue> &issues);

result issue_statuses(redmine::config &config, redmine::options &options,
                      std::vector<issue_status> &issue_statuses);

result issue_categories(const std::string &project, redmine::config &config,
                        redmine::options &options,
                        std::vector<issue_category> &issue_categories);
}
}

#endif
