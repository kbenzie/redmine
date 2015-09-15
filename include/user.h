#ifndef REDMINE_USER_H
#define REDMINE_USER_H

#include <config.h>
#include <redmine.h>

#include <json/json.hpp>

#include <vector>

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

namespace action {
result user(int argc, char **argv, options options);
result user_list(int argc, char **argv, options options);
result user_show(int argc, char **argv, options options);
}

namespace query {
result users(config &config, options options, std::vector<user> &out);
}
}

#endif
