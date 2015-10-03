#ifndef ISSUE_H
#define ISSUE_H

#include <command_line.h>
#include <config.h>
#include <redmine.h>
#include <user.h>

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
  /// @return Return either redmine::SUCCESS or redmine::FAILURE.
  result init(const json::object &object);

  /// @brief Query and initilise issue from id.
  ///
  /// @param id Issue id to get.
  /// @param config User configuation.
  /// @param options Command line options.
  ///
  /// @return Return either redmine::SUCCESS or redmine::FAILURE.
  result get(const uint32_t id, const redmine::config &config,
             redmine::options &options);

  /// @brief Query and initilise issue from id.
  ///
  /// @param id String issue id to get.
  /// @param config User configuation.
  /// @param options Command line options.
  ///
  /// @return Return either redmine::SUCCESS or redmine::FAILURE.
  result get(const std::string &id, const redmine::config &config,
             redmine::options &options);

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

  struct journal {
    std::string created_on;
    struct detail {
      std::string name;
      std::string new_value;
      std::string old_value;
      std::string property;
    };
    uint32_t id;
    std::string notes;
    reference user;
  };
  std::vector<journal> journals;

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
result issue(redmine::cl::args &args, redmine::config &config,
             redmine::current_user &user, redmine::options &options);
result issue_list(redmine::cl::args &args, redmine::config &config,
                  redmine::options &options);
result issue_new(redmine::cl::args &args, redmine::config &config,
                 redmine::current_user &user, redmine::options &options);
result issue_show(redmine::cl::args &args, redmine::config &config,
                  redmine::options &options);
result issue_update(redmine::cl::args &args, redmine::config &config,
                    redmine::current_user &user, redmine::options &options);
}  // action

namespace query {
result issues(std::string &filter, redmine::config &config,
              redmine::options &options, std::vector<issue> &issues);

result issue_statuses(redmine::config &config, redmine::options &options,
                      std::vector<issue_status> &issue_statuses);

result issue_categories(const std::string &project, redmine::config &config,
                        redmine::options &options,
                        std::vector<issue_category> &issue_categories);
}  // query
}  // redmine

#endif
