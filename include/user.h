// Copyright (C) 2015 Kenenth Benzie
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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
