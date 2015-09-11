#ifndef REDMINE_USER_H
#define REDMINE_USER_H

#include <config.h>
#include <redmine.h>

#include <json/json.hpp>

#include <vector>

struct user_t {
  std::string firstname;
  std::string lastname;
  uint32_t id;
  std::string mail;
  std::string login;
  std::string api_key;
  std::string created_on;
  std::string last_login_on;
  uint32_t status;
};

namespace action {
result_t user(int argc, char **argv, options_t options);
result_t user_list(int argc, char **argv, options_t options);
result_t user_show(int argc, char **argv, options_t options);
}

result_t user_serialize(const user_t &user, std::string &out);

result_t user_deserialize(const json::object &user, user_t &out);

result_t user_list_fetch(config_t &config, options_t options,
                         std::vector<user_t> &out);

#endif
