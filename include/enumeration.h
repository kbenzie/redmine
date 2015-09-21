#ifndef REDMINE_ENUMERATIONS_H
#define REDMINE_ENUMERATIONS_H

#include <redmine.h>
#include <config.h>

namespace redmine {
struct enumeration {
  uint32_t id;
  std::string name;
  bool is_default;
};

namespace query {
/// @brief Query the list of redmine issue priorities.
///
/// @param config User configuration.
/// @param options Command line options.
/// @param issue_statuses Returned list of issue statuses.
///
/// @return Returns SUCCESS or FAILURE.
result issue_priorities(redmine::config &config, redmine::options &options,
                        std::vector<enumeration> &issue_statuses);

/// @brief Query the list of redmine time entry activities.
///
/// @param config User configuration.
/// @param options Command line options.
/// @param time_entry_activities Returned list of time entry activities.
///
/// @return Returns SUCCESS or FAILURE.
result time_entry_activities(redmine::config &config, redmine::options &options,
                             std::vector<enumeration> &time_entry_activities);
}
}

#endif
