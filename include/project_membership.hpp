#ifndef REDMINE_PROJECT_MEMBERSHIP_HPP
#define REDMINE_PROJECT_MEMBERSHIP_HPP

#include <config.h>
#include <redmine.h>

namespace redmine {
struct project_membership {
  project_membership();

  result init(const json::object &object);

  uint32_t id;
  reference project;
  reference user;
  std::vector<reference> roles;
};

namespace query {
result project_memberships(
    const std::string &project, config &config, options options,
    std::vector<project_membership> &project_memberships);
}
}

#endif
