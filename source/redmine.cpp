#include <config.h>
#include <issue.h>
#include <http.h>
#include <project.h>
#include <redmine.h>

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
            "options:\n"
            "        -V - verbose output\n"
            "        --debug - enable debug output\n");
    return FAILURE;
  }

  http::session http;

  options_t options = NONE;
  int argi = 1;
  for (; argi < argc; ++argi) {
    if (!strcmp("-V", argv[argi])) {
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
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return INVALID_ARGUMENT;
}
