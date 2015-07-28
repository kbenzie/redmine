#include <project.h>

#include <cstdio>
#include <cstring>

result cmd::project(int argc, char **argv) {
  if (0 == argc) {
    fprintf(stderr, "usage: redmine project <action> [args]\n"
                    "actions:\n"
                    "        info\n"
                    "        list\n");
    return FAILURE;
  }

  if (!strcmp("info", argv[0])) {
    return project_info(argc - 1, argv + 1);
  }

  if (!strcmp("list", argv[0])) {
    return project_list(argc - 1, argv + 1);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result cmd::project_info(int argc, char **argv) {
  fprintf(stderr, "unsupported: project info\n");
  return UNSUPPORTED;
}

result cmd::project_list(int argc, char **argv) {
  fprintf(stderr, "unsupported: project list\n");
  return UNSUPPORTED;
}
