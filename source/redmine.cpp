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
    return FAILURE;
  }

  http::session http;
  CHECK(http.init(), return FAILURE);

  options_t options = NONE;
  int argi = 1;
  for (; argi < argc; ++argi) {
    if (!strcmp("--verbose", argv[argi])) {
      options |= VERBOSE;
      CHECK(argc - 1 == argi, fprintf(stderr, "action required\n");
            return ACTION_REQUIRED);
      continue;
    }

    if (!strcmp("--debug", argv[argi])) {
      options |= DEBUG;
      CHECK(argc - 1 == argi, fprintf(stderr, "action required\n");
            return ACTION_REQUIRED);
      continue;
    }

    if (!strcmp("--debug-http", argv[argi])) {
      options |= DEBUG_HTTP;
      CHECK(argc - 1 == argi, fprintf(stderr, "action required\n");
            return ACTION_REQUIRED);
      continue;
    }

    if (!strcmp("config", argv[argi])) {
      ++argi;
      return action::config(argc - argi, argv + argi, options);
    }

    if (!strcmp("project", argv[argi])) {
      ++argi;
      return action::project(argc - argi, argv + argi, options);
    }

    if (!strcmp("issue", argv[argi])) {
      ++argi;
      return action::issue(argc - argi, argv + argi, options);
    }

    if (!strcmp("user", argv[argi])) {
      ++argi;
      return action::user(argc - argi, argv + argi, options);
    }
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return INVALID_ARGUMENT;
}

#ifdef REDMINE_DEBUG
const char *result_string(result_t result) {
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

result_t reference_deserialize(const json::object &ref, reference_t &out) {
  auto name = ref.get("name");
  CHECK_JSON_PTR(name, json::TYPE_STRING);

  auto id = ref.get("id");
  CHECK_JSON_PTR(id, json::TYPE_NUMBER);

  out.name = name->string();
  out.id = id->number<uint32_t>();

  return SUCCESS;
}
