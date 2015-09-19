#ifndef REDMINE_MEMBERSHIP_H
#define REDMINE_MEMBERSHIP_H

#include <config.h>
#include <redmine.h>

namespace redmine {
struct membership {
  membership();

  result init(const json::object &object);

  uint32_t id;
  reference project;
  reference user;
  std::vector<reference> roles;
};

namespace query {
result memberships(const std::string &project, config &config, options options,
                   std::vector<membership> &memberships);
}
}

#endif
