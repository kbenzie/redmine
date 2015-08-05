#include <config.h>
#include <http.h>
#include <project.h>

#include <json/json.hpp>

#include <cstdio>
#include <cstring>

namespace action {
result_t project(int argc, char **argv, options_t options) {
  if (0 == argc) {
    fprintf(stderr,
            "usage: redmine project <action> [args]\n"
            "actions:\n"
            "        show <id|name>\n"
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
  CHECK(0 == argc, fprintf(stderr, "missing id or name\n"); return FAILURE);
  CHECK(1 < argc, fprintf(stderr, "invalid argument: %s\n", argv[1]));

  config_t config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  // TODO: Lookup projects for name and get id
  std::string id(argv[0]);

  std::string body;
  CHECK_RETURN(http::post(config.url + std::string("/projects/") + id + ".json",
                          config.key, options, body));

  json::value root = json::read(body, false);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto &project = root.object().get("project")->object();

  printf("id: %d\n", static_cast<int>(project.get("id")->number()));
  printf("name: %s\n", project.get("name")->string().c_str());
  printf("description: %s\n", project.get("description")->string().c_str());

  if (has<VERBOSE>(options)) {
    printf("identifier: %s\n", project.get("identifier")->string().c_str());
    printf("homepage: %s\n", project.get("homepage")->string().c_str());
    printf("created_on: %s\n", project.get("created_on")->string().c_str());
    printf("updated_on: %s\n", project.get("updated_on")->string().c_str());
    printf("status: %d\n", static_cast<int>(project.get("status")->number()));
  }

  return SUCCESS;
}

result_t project_list(int argc, char **argv, options_t options) {
  CHECK(0 != argc, fprintf(stderr, "invalid argument: %s\n", argv[0]);
        return INVALID_ARGUMENT);

  config_t config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::vector<project_t> projects;
  CHECK_RETURN(project_list_fetch(config, options, projects));

  for (auto &project : projects) {
    printf("%d: %s\n", project.id, project.name.c_str());
    if (project.parent.id) {
      printf(" '- %d: %s\n", project.parent.id, project.parent.name.c_str());
    }
  }

  return SUCCESS;
}
}

result_t project_list_fetch(config_t &config, options_t options,
                            std::vector<project_t> &out) {
  std::string body;
  CHECK_RETURN(http::post(std::string(config.url + "/projects.json").c_str(),
                          config.key.c_str(), options, body));

  auto root = json::read(body, false);
  CHECK(json::TYPE_OBJECT != root.type(),
        fprintf(stderr, "invalid json data: %s\n", body.c_str());
        return FAILURE);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

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

#define CHECK_PROJECT_PARAM(PARAM, TYPE)                                       \
  json::value *PARAM = project.object().get(#PARAM);                           \
  CHECK(!PARAM,                                                                \
        fprintf(stderr, "invalid json data: project %s not found\n", #PARAM);  \
        CHECK(has<VERBOSE>(options),                                           \
              fprintf(stderr, "%s\n", json::write(root, "  ").c_str()));       \
        return FAILURE);                                                       \
  CHECK(TYPE != PARAM->type(),                                                 \
        fprintf(stderr, "invalid json data: project %s is not a %s\n", #PARAM, \
                #TYPE);                                                        \
        CHECK(has<VERBOSE>(options),                                           \
              fprintf(stderr, "%s\n", json::write(root, "  ").c_str()));       \
        return FAILURE)

    CHECK_PROJECT_PARAM(name, json::TYPE_STRING);
    CHECK_PROJECT_PARAM(id, json::TYPE_NUMBER);
    CHECK_PROJECT_PARAM(identifier, json::TYPE_STRING);
    CHECK_PROJECT_PARAM(description, json::TYPE_STRING);
    CHECK_PROJECT_PARAM(created_on, json::TYPE_STRING);
    CHECK_PROJECT_PARAM(updated_on, json::TYPE_STRING);
    CHECK_PROJECT_PARAM(status, json::TYPE_NUMBER);

#undef CHECK_PROJECT_PARAM

    json::value *parent = project.object().get("parent");
    std::string parent_name;
    int parent_id = 0;
    if (parent) {
      CHECK(json::TYPE_OBJECT != parent->type(),
            fprintf(stderr,
                    "invalid json data: project parent is not an object\n");
            CHECK(has<VERBOSE>(options),
                  fprintf(stderr, "%s\n", json::write(root, "  ").c_str()));
            return FAILURE);

#define CHECK_PARENT_PARAM(PARAM, TYPE)                                       \
  json::value *PARAM = parent->object().get(#PARAM);                          \
  CHECK(!PARAM,                                                               \
        fprintf(stderr, "invalid json data: project parent %s not found\n",   \
                #PARAM);                                                      \
        CHECK(has<VERBOSE>(options),                                          \
              fprintf(stderr, "%s\n", json::write(root, "  ").c_str()));      \
        return FAILURE);                                                      \
  CHECK(TYPE != PARAM->type(),                                                \
        fprintf(stderr, "invalid json data: project parent %s is not a %s\n", \
                #PARAM, #TYPE);                                               \
        CHECK(has<VERBOSE>(options),                                          \
              fprintf(stderr, "%s\n", json::write(root, "  ").c_str()));      \
        return FAILURE)

      CHECK_PARENT_PARAM(name, json::TYPE_STRING);
      CHECK_PARENT_PARAM(id, json::TYPE_NUMBER);

      parent_name = name->string();
      parent_id = static_cast<int>(id->number());

#undef CHECK_PARENT_PARAM
    }

    out.push_back({name->string(),
                   static_cast<int>(id->number()),
                   identifier->string(),
                   created_on->string(),
                   updated_on->string(),
                   description->string(),
                   static_cast<int>(status->number()),
                   {parent_name.c_str(), parent_id}});
  }

  return SUCCESS;
}
