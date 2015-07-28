#include <config.h>

#include <cstdio>
#include <cstring>

result config(int argc, char **argv) {
  if (0 == argc) {
    fprintf(stderr, "usage: redmine config <action> [args]\n"
                    "actions:\n"
                    "        key [new key]\n"
                    "        url [new url]\n");
    return FAILURE;
  }

  if (!strcmp("key", argv[0])) {
    return config_key(argc - 1, argv + 1);
  }

  if (!strcmp("url", argv[0])) {
    return config_url(argc - 1, argv + 1);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result config_key(int argc, char **argv) {
  fprintf(stderr, "unsupported: config key\n");
  return UNSUPPORTED;
}

result config_url(int argc, char **argv) {
  fprintf(stderr, "unsupported: config url\n");
  return UNSUPPORTED;
}
