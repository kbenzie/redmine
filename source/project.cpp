#include <project.h>

#include <cstdio>
#include <cstring>

result cmd::project(int argc, char **argv) {
  if (0 == argc) {
    fprintf(stderr, "usage: redmine project <action> [args]\n"
                    "actions:\n"
                    "        show\n"
                    "        list\n");
    return FAILURE;
  }

  if (!strcmp("show", argv[0])) {
    return project_show(argc - 1, argv + 1);
  }

  if (!strcmp("list", argv[0])) {
    return project_list(argc - 1, argv + 1);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result cmd::project_show(int argc, char **argv) {
  fprintf(stderr, "unsupported: project show\n");
  return UNSUPPORTED;
}

result cmd::project_list(int argc, char **argv) {
  fprintf(stderr, "unsupported: project list\n");
  return UNSUPPORTED;
}
