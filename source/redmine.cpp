#include <config.h>
#include <issue.h>
#include <http.h>
#include <project.h>
#include <redmine.h>
#include <user.h>

#include <cstdio>
#include <cstring>

int main(int argc, char **argv) {
  if (1 == argc) {
    fprintf(stderr,
            "usage: redmine [options] <action> [args]\n"
            "actions:\n"
            "        config\n"
            "        project\n"
            "        issue\n"
            "        user\n"
            "options:\n"
            "        --verbose - verbose output\n"
            "        --debug - enable debug output\n"
            "        --debug-http - enable http debug output\n");
    return redmine::FAILURE;
  }

  redmine::http::session http;
  CHECK(http.init(), return redmine::FAILURE);

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

    if (!strcmp("config", argv[argi])) {
      ++argi;
      return redmine::action::config(argc - argi, argv + argi, options);
    }

    if (!strcmp("project", argv[argi])) {
      ++argi;
      return redmine::action::project(argc - argi, argv + argi, options);
    }

    if (!strcmp("issue", argv[argi])) {
      ++argi;
      return redmine::action::issue(argc - argi, argv + argi, options);
    }

    if (!strcmp("user", argv[argi])) {
      ++argi;
      return redmine::action::user(argc - argi, argv + argi, options);
    }
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
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
