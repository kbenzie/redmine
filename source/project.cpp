#include <config.h>
#include <project.h>
#include <request.h>

#include <cstdio>
#include <cstring>

namespace action {
result_t project(int argc, char **argv, options_t options) {
  if (0 == argc) {
    fprintf(stderr,
            "usage: redmine project <action> [args]\n"
            "actions:\n"
            "        show\n"
            "        list\n");
    return FAILURE;
  }

  if (!strcmp("show", argv[0])) {
    return project_show(argc - 1, argv + 1, options);
  }

  if (!strcmp("list", argv[0])) {
    return project_list(argc - 1, argv + 1, options);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result_t project_show(int argc, char **argv, options_t options) {
  fprintf(stderr, "unsupported: project show\n");
  return UNSUPPORTED;
}

result_t project_list(int argc, char **argv, options_t options) {
  if (0 != argc) {
    fprintf(stderr, "invalid argument: %s\n", argv[0]);
    return INVALID_ARGUMENT;
  }

  config_t config;
  CHECK(config_load(&config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::string url = config.url + "/projects.json";
  std::stringstream body;
  CHECK_RETURN(request(url.c_str(), config.key.c_str(), options, body));

  return UNSUPPORTED;
}
}
