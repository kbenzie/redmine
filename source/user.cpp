#include <http.h>
#include <user.h>

#include <json/json.hpp>

namespace redmine {
user::user() {}

result user::init(const json::object &object) {
  auto Firstname = object.get("firstname");
  CHECK_JSON_PTR(Firstname, json::TYPE_STRING);
  firstname = Firstname->string();

  auto Lastname = object.get("lastname");
  CHECK_JSON_PTR(Lastname, json::TYPE_STRING);
  lastname = Lastname->string();

  name = firstname + " " + lastname;

  auto Id = object.get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
  id = Id->number<uint32_t>();

  auto Mail = object.get("mail");
  CHECK_JSON_PTR(Mail, json::TYPE_STRING);
  mail = Mail->string();

  auto Login = object.get("login");
  CHECK_JSON_PTR(Login, json::TYPE_STRING);
  login = Login->string();

  auto ApiKey = object.get("api_key");
  if (ApiKey) {
    CHECK_JSON_TYPE((*ApiKey), json::TYPE_STRING);
    api_key = ApiKey->string();
  }

  auto CreatedOn = object.get("created_on");
  CHECK_JSON_PTR(CreatedOn, json::TYPE_STRING);
  created_on = CreatedOn->string();

  auto LastLoginOn = object.get("last_login_on");
  CHECK_JSON_PTR(LastLoginOn, json::TYPE_STRING);
  last_login_on = LastLoginOn->string();

  auto Status = object.get("status");
  if (Status) {
    CHECK_JSON_PTR(Status, json::TYPE_NUMBER);
    status = Status->number<uint32_t>();
  }

  return SUCCESS;
}

namespace action {
result user(int argc, char **argv, options options) {
  if (0 == argc) {
    fprintf(stderr,
            "usage: redmine issue <action> [args]\n"
            "actions:\n"
            "        list\n"
            "        show <id>\n");
    return FAILURE;
  }

  if (!strcmp("list", argv[0])) {
    return user_list(argc - 1, argv + 1, options);
  }

  if (!strcmp("show", argv[0])) {
    return user_show(argc - 1, argv + 1, options);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result user_list(int argc, char **argv, options options) {
  CHECK(argc, fprintf(stderr, "invalid argument: %s\n", argv[0]);
        return FAILURE);

  redmine::config config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::vector<redmine::user> users;
  CHECK_RETURN(query::users(config, options, users));

  printf(
      "  id | name\n"
      "-----|------------------------------------------------------------------"
      "--------\n");
  for (auto &user : users) {
    printf("%4d | %s %s\n", user.id, user.firstname.c_str(),
           user.lastname.c_str());
  }

  return SUCCESS;
}

result user_show(int argc, char **argv, options options) {
  CHECK(0 == argc, fprintf(stderr, "missing id\n"));
  CHECK(1 < argc, fprintf(stderr, "invalid argument: %s\n", argv[1]));

  redmine::config config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::string body;
  CHECK_RETURN(http::get("/users/" + std::string(argv[0]) + ".json", config,
                         options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(has<DEBUG>(options), printf("%s\n", json::write(Root, "  ").c_str()));

  auto User = Root.object().get("user");
  CHECK_JSON_PTR(User, json::TYPE_OBJECT);

  redmine::user user;
  CHECK_RETURN(user.init(User->object()));

  printf("      name: %s %s\n", user.firstname.c_str(), user.lastname.c_str());
  printf("        id: %u\n", user.id);
  printf("     email: %s\n", user.mail.c_str());
  printf("     login: %s\n", user.login.c_str());
  if (!user.api_key.empty()) {
    printf("   api key: %s\n", user.api_key.c_str());
  }
  printf("   created: %s\n", user.created_on.c_str());
  printf("last login: %s\n", user.last_login_on.c_str());
  printf("    status: %u\n", user.status);

  return SUCCESS;
}
}

result query::users(config &config, options options, std::vector<user> &out) {
  std::string body;
  CHECK_RETURN(http::get("/users.json", config, options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(Root, "  ").c_str()));

  auto Users = Root.object().get("users");
  CHECK_JSON_PTR(Users, json::TYPE_ARRAY);

  for (auto &User : Users->array()) {
    CHECK_JSON_TYPE(User, json::TYPE_OBJECT);

    redmine::user user;
    CHECK_RETURN(user.init(User.object()));

    out.push_back(user);
  }

  return SUCCESS;
}
}
