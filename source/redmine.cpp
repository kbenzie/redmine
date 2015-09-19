#include <redmine.h>
#include <config.h>
#include <issue.h>
#include <http.h>
#include <project.h>
#include <user.h>

#include <cstdio>
#include <cstring>

int main(int argc, char **argv) {
  redmine::options options = redmine::NONE;
  int argi = 1;
  for (; argi < argc; ++argi) {
    if (!strcmp("--verbose", argv[argi])) {
      options |= redmine::VERBOSE;
      CHECK(argc - 1 == argi, fprintf(stderr, "action required\n");
            return redmine::ACTION_REQUIRED);
      continue;
    }

    if (!strcmp("--debug", argv[argi])) {
      options |= redmine::DEBUG;
      CHECK(argc - 1 == argi, fprintf(stderr, "action required\n");
            return redmine::ACTION_REQUIRED);
      continue;
    }

    if (!strcmp("--debug-http", argv[argi])) {
      options |= redmine::DEBUG_HTTP;
      CHECK(argc - 1 == argi, fprintf(stderr, "action required\n");
            return redmine::ACTION_REQUIRED);
      continue;
    }

    break;
  }

  redmine::http::session http;
  CHECK_RETURN(http.init());

  redmine::config config;
  CHECK_RETURN(config.load(options));

  redmine::current_user user;
  CHECK_RETURN(user.get(config, options));

  // TODO: (Dis)enable actions based on current user permissions.
  if (1 == argc) {
    printf(
        "usage: redmine [options] <action> [args]\n"
        "actions:\n");
    printf("        config\n");
    if (user.can(redmine::USE_PROJECT)) {
      printf("        project\n");
    }
    if (user.can(redmine::USE_ISSUE)) {
      printf("        issue\n");
    }
#if 0
    if (user.permissions.user) {
      printf("        user\n");
    }
#endif
    printf(
        "options:\n"
        "        --verbose - verbose output\n"
        "        --debug - enable debug output\n");
#if REDMINE_DEBUG
    printf("        --debug-http - enable http debug output\n");
#endif

    return redmine::SUCCESS;
  }

  for (; argi < argc; ++argi) {
    if (!strcmp("config", argv[argi])) {
      ++argi;
      return redmine::action::config(argc - argi, argv + argi, options);
    }

    if (!strcmp("project", argv[argi]) && user.can(redmine::USE_PROJECT)) {
      ++argi;
      return redmine::action::project(argc - argi, argv + argi, config,
                                      options);
    }

    if (!strcmp("issue", argv[argi]) && user.can(redmine::USE_ISSUE)) {
      ++argi;
      return redmine::action::issue(argc - argi, argv + argi, config, options);
    }

#if 0
    if (!strcmp("user", argv[argi])) {
      ++argi;
      return redmine::action::user(argc - argi, argv + argi, config, options);
    }
#endif

    fprintf(stderr, "invalid action: %s\n", argv[argi]);
    return redmine::FAILURE;
  }

  fprintf(stderr, "invalid argument: %s\n", argv[argi]);
  return redmine::INVALID_ARGUMENT;
}

#ifdef REDMINE_DEBUG
const char *redmine::result_string(result result) {
  switch (result) {
    case SUCCESS:
      return "success";
    case FAILURE:
      return "failure";
    case UNSUPPORTED:
      return "unsupported";
    case ACTION_REQUIRED:
      return "action required";
    case INVALID_ARGUMENT:
      return "invalid argument";
    case INVALID_CONFIG:
      return "invalid config";
  }
}
#endif

redmine::result redmine::reference::init(const json::object &object) {
  auto Id = object.get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);

  auto Name = object.get("name");
  CHECK_JSON_PTR(Name, json::TYPE_STRING);

  id = Id->number<uint32_t>();
  name = Name->string();

  return SUCCESS;
}
