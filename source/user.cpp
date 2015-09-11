#include <http.h>
#include <user.h>

#include <json/json.hpp>

namespace action {
result_t user(int argc, char **argv, options_t options) {
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

result_t user_list(int argc, char **argv, options_t options) {
  CHECK(argc, fprintf(stderr, "invalid argument: %s\n", argv[0]);
        return FAILURE);

  config_t config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::vector<user_t> users;
  CHECK_RETURN(user_list_fetch(config, options, users));

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

result_t user_show(int argc, char **argv, options_t options) {
  CHECK(0 == argc, fprintf(stderr, "missing id\n"));
  CHECK(1 < argc, fprintf(stderr, "invalid argument: %s\n", argv[1]));

  config_t config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::string body;
  CHECK_RETURN(http::get("/users/" + std::string(argv[0]) + ".json", config,
                         options, body));

  auto root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);
  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto user = root.object().get("user");
  CHECK_JSON_PTR(user, json::TYPE_OBJECT);

  user_t U;
  CHECK_RETURN(user_deserialize(user->object(), U));

  printf("      name: %s %s\n", U.firstname.c_str(), U.lastname.c_str());
  printf("        id: %u\n", U.id);
  printf("     email: %s\n", U.mail.c_str());
  printf("     login: %s\n", U.login.c_str());
  if (!U.api_key.empty()) {
    printf("   api key: %s\n", U.api_key.c_str());
  }
  printf("   created: %s\n", U.created_on.c_str());
  printf("last login: %s\n", U.last_login_on.c_str());
  printf("    status: %u\n", U.status);

  return SUCCESS;
}
}

result_t user_deserialize(const json::object &user, user_t &out) {
  auto firstname = user.get("firstname");
  CHECK_JSON_PTR(firstname, json::TYPE_STRING);
  out.firstname = firstname->string();

  auto lastname = user.get("lastname");
  CHECK_JSON_PTR(lastname, json::TYPE_STRING);
  out.lastname = lastname->string();

  auto id = user.get("id");
  CHECK_JSON_PTR(id, json::TYPE_NUMBER);
  out.id = id->number<uint32_t>();

  auto mail = user.get("mail");
  CHECK_JSON_PTR(mail, json::TYPE_STRING);
  out.mail = mail->string();

  auto login = user.get("login");
  CHECK_JSON_PTR(login, json::TYPE_STRING);
  out.login = login->string();

  auto api_key = user.get("api_key");
  if (api_key) {
    CHECK_JSON_TYPE((*api_key), json::TYPE_STRING);
    out.api_key = api_key->string();
  }

  auto created_on = user.get("created_on");
  CHECK_JSON_PTR(created_on, json::TYPE_STRING);
  out.created_on = created_on->string();

  auto last_login_on = user.get("last_login_on");
  CHECK_JSON_PTR(last_login_on, json::TYPE_STRING);
  out.last_login_on = last_login_on->string();

  auto status = user.get("status");
  if (status) {
    CHECK_JSON_PTR(status, json::TYPE_NUMBER);
    out.status = status->number<uint32_t>();
  }

  return SUCCESS;
}

result_t user_list_fetch(config_t &config, options_t options,
                         std::vector<user_t> &out) {
  std::string body;
  CHECK_RETURN(http::get("/users.json", config, options, body));

  auto root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto users = root.object().get("users");
  CHECK_JSON_PTR(users, json::TYPE_ARRAY);

  for (auto &user : users->array()) {
    CHECK_JSON_TYPE(user, json::TYPE_OBJECT);

    user_t U;
    CHECK_RETURN(user_deserialize(user.object(), U));

    out.push_back(U);
  }

  return SUCCESS;
}
