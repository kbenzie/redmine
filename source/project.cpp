#include <config.h>
#include <project.h>
#include <request.h>

#include <json/json.hpp>

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

  std::string body;
  CHECK_RETURN(request(std::string(config.url + "/projects.json").c_str(),
                       config.key.c_str(), options, body));



  auto root = json::read(body, false);
  CHECK(json::TYPE_OBJECT != root.type(),
        fprintf(stderr, "invalid json data: %s\n", body.c_str());
        return FAILURE);

  auto projects = root.object().get("projects");
  CHECK(!projects,
        fprintf(stderr, "invalid json data: projects array not found\n");
        CHECK(has<VERBOSE>(options),
              fprintf(stderr, "%s\n", json::write(body, "  ").c_str()));
        return FAILURE);
  CHECK(json::TYPE_ARRAY != projects->type(),
        fprintf(stderr, "invalid json data: projects to an array\n");
        CHECK(has<VERBOSE>(options),
              fprintf(stderr, "%s\n", json::write(body, "  ").c_str()));
        return FAILURE);

  for (auto project : projects->array()) {
    CHECK(json::TYPE_OBJECT != project.type(),
          fprintf(stderr, "invalid json data: project is not an object\n");
          CHECK(has<VERBOSE>(options),
                fprintf(stderr, "%s\n", json::write(root, "  ").c_str()));
          return FAILURE);

    auto id = project.object().get("id");
    CHECK(!id, fprintf(stderr, "invalid json data: project id not found\n");
          CHECK(has<VERBOSE>(options),
                fprintf(stderr, "%s\n", json::write(root, "  ").c_str()));
          return FAILURE);
    CHECK(json::TYPE_NUMBER != id->type(),
          fprintf(stderr, "invalid json data: project id is not a string\n");
          CHECK(has<VERBOSE>(options),
                fprintf(stderr, "%s\n", json::write(root, "  ").c_str()));
          return FAILURE);

    auto name = project.object().get("name");
    CHECK(!name, fprintf(stderr, "invalid json data: project name not found\n");
          CHECK(has<VERBOSE>(options),
                fprintf(stderr, "%s\n", json::write(root, "  ").c_str()));
          return FAILURE);
    CHECK(json::TYPE_STRING != name->type(),
          fprintf(stderr, "invalid json data: project name is not a string\n");
          CHECK(has<VERBOSE>(options),
                fprintf(stderr, "%s\n", json::write(root, "  ").c_str()));
          return FAILURE);

    printf("%d: %s\n", static_cast<int>(id->number()), name->string().c_str());
  }

  return UNSUPPORTED;
}
}
