#include <config.h>
#include <issue.h>
#include <project.h>

#include <cstdio>
#include <cstring>

int main(int argc, char **argv) {
  if (1 == argc) {
    fprintf(stderr, "usage: redmine [options] <action> [args]\n"
                    "actions:\n"
                    "        config\n"
                    "        project\n"
                    "        issue\n");
    return FAILURE;
  }

  if (!strcmp("config", argv[1])) {
    return cmd::config(argc - 2, argv + 2);
  }

  if (!strcmp("project", argv[1])) {
    return cmd::project(argc - 2, argv + 2);
  }

  if (!strcmp("issue", argv[1])) {
    return cmd::issue(argc - 2, argv + 2);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return INVALID_ARGUMENT;
}
