#ifndef REDMINE_USER_H
#define REDMINE_USER_H

#include <role.h>
#include <config.h>

#include <json/json.hpp>

#include <vector>
#include <unordered_map>

namespace redmine {
struct user {
  user();

  result init(const json::object &object);

  json::object jsonify();

  uint32_t id;
  std::string firstname;
  std::string lastname;
  std::string name;
  std::string mail;
  std::string login;
  std::string api_key;
  std::string created_on;
  std::string last_login_on;
  uint32_t status;
};

struct current_user {
  current_user();

  result get(redmine::config &config, redmine::options &options);

  bool can(redmine::permisson permisson);

  uint32_t id;
  std::string login;
  std::string firstname;
  std::string lastname;
  std::string mail;
  std::string created_on;
  std::string last_login_on;
  uint32_t status;  // NOTE: Only the admin can see this.
  struct membership {
    reference project;
    std::vector<reference> roles;
  };
  std::vector<membership> memberships;
  std::unordered_map<uint32_t, redmine::permissions> project_permissions;
  redmine::permissions permissions;
};

namespace action {
result user(redmine::cl::args &args, redmine::config &config,
            redmine::options &options);
result user_list(redmine::cl::args &args, redmine::config &config,
                 redmine::options &options);
result user_show(redmine::cl::args &args, redmine::config &config,
                 redmine::options &options);
}

namespace query {
result users(redmine::config &config, redmine::options &options,
             std::vector<user> &out);
}
}

#endif
