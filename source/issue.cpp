#include <issue.h>

#include <cstdio>
#include <cstring>

result issue(int argc, char **argv) {
  if (0 == argc) {
    fprintf(stderr, "usage: redmine issue <action> [args]\n"
                    "actions:\n"
                    "        new\n"
                    "        edit\n"
                    "        list\n");
    return FAILURE;
  }

  if (!strcmp("new", argv[0])) {
    return issue_new(argc - 1, argv + 1);
  }

  if (!strcmp("edit", argv[0])) {
    return issue_edit(argc - 1, argv + 1);
  }

  if (!strcmp("list", argv[0])) {
    return issue_list(argc - 1, argv + 1);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result issue_new(int argc, char **argv) {
  fprintf(stderr, "unsupported: issue new\n");
  return UNSUPPORTED;
}

result issue_edit(int argc, char **argv) {
  fprintf(stderr, "unsupported: issue edit\n");
  return UNSUPPORTED;
}

result issue_list(int argc, char **argv) {
  fprintf(stderr, "unsupported: issue list\n");
  return UNSUPPORTED;
}
