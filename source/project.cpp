#include <config.h>
#include <http.h>
#include <project.h>
#include <util.h>

#include <json/json.hpp>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sstream>

namespace action {
result_t project(int argc, char **argv, options_t options) {
  if (0 == argc) {
    fprintf(stderr,
            "usage: redmine project <action> [args]\n"
            "actions:\n"
            "        new <name> <identifier>\n"
            "        show <id|name>\n"
            "        list\n");
    return FAILURE;
  }

  if (!strcmp("new", argv[0])) {
    return project_new(argc - 1, argv + 1, options);
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

result_t project_new(int argc, char **argv, options_t options) {
  CHECK(2 > argc,
        fprintf(stderr, "usage: redmine project new <name> <identifier>\n");
        return FAILURE);
  if (2 < argc) {
    fprintf(stderr, "invalid arguments:");
    for (int argi = 0; argi < argc; ++argi) {
      fprintf(stderr, " %s\n", argv[argi]);
    }
    return FAILURE;
  };

  config_t config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::string filename = util::getcwd();
  filename += "/REDMINE_PROJECT_NEW";
  CHECK(has<DEBUG>(options), printf("%s\n", filename.c_str()));
  {
    // NOTE: Populate the REDMINE_PROJECT_NEW temporary file
    std::ofstream file(filename);
    CHECK(!file.is_open(),
          fprintf(stderr, "could not create temporary file: %s\n",
                  filename.c_str());
          return FAILURE);
    file << "name: " << argv[0] << "\n";
    file << "identifier: " << argv[1] << "\n";
    file << "description: \n";
    file << "homepage: \n";
    file << "is_public: true\n";
    file << "parent_id: \n";  // the parent project number
    file << "inherit_members: false\n";
    file << "tracker_ids: \n";  // (repeatable element) the tracker id: 1 for
                                // Bug, etc.
    file << "enabled_module_names: boards, calendar, documents, files, gantt, "
            "issue_tracking, news, repository, time_tracking, wiki\n";
  }

  // TODO: Make editor configureable
  std::string editor("vim");

  // TODO: Open REDMINE_PROJECT_NEW in editor
  std::string command = editor + " " + filename;
  int result = std::system(command.c_str());
  CHECK(result, fprintf(stderr, "failed to load editor: %s", editor.c_str());
        return FAILURE);

  std::stringstream content;
  {
    // NOTE: Read user content of REDMINE_PROJECT_NEW file
    std::ifstream file(filename);
    CHECK(!file.is_open(),
          fprintf(stderr, "could not open temporary file: %s\n",
                  filename.c_str());
          return FAILURE);
    std::copy(std::istreambuf_iterator<char>(file),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(content));
    // NOTE: Remove temporary file REDMINE_PROJECT_NEW
    util::rm(filename);
  }

  json::object project;

  // TODO: Validate content of REDMINE_PROJECT_NEW
  auto strip = [](const std::string &str, const char *prefix) -> std::string {
    size_t len = std::strlen(prefix);
    if (str.size() > len) {
      return str.substr(len, str.size());
    } else {
      return "";
    }
  };

  std::string name;
  std::getline(content, name);
  project.add("name", strip(name, "name: "));

  std::string identifier;
  std::getline(content, identifier);
  identifier = strip(identifier, "identifier: ");
  project.add("identifier", identifier);

  std::string description;
  std::getline(content, description);
  description = strip(description, "description: ");
  if (!description.empty()) {
    project.add("description", description);
  }

  std::string homepage;
  std::getline(content, homepage);
  homepage = strip(homepage, "homepage: ");
  if (!homepage.empty()) {
    project.add("homepage", homepage);
  }

  std::string is_public;
  std::getline(content, is_public);
  is_public = strip(is_public, "is_public: ");
  if (!is_public.empty()) {
    project.add("is_public", is_public);
  }

  std::string parent_id;
  std::getline(content, parent_id);
  parent_id = strip(parent_id, "parent_id: ");
  if (!parent_id.empty()) {
    project.add("parent_id", parent_id);
  }

  std::string inherit_members;
  std::getline(content, inherit_members);
  inherit_members = strip(inherit_members, "inherit_members: ");
  if (!inherit_members.empty()) {
    project.add("inherit_members", inherit_members);
  }

#if 0
  std::string tracker_ids;
  std::getline(content, tracker_ids);
  //"tracker_ids: \n";  // (repeatable element) the tracker id: 1 for Bug, etc.

  std::string enabled_module_names;
  std::getline(content, enabled_module_names);
//"enabled_module_names: boards, calendar, documents, files, gantt,
// issue_tracking, news, repository, time_tracking, wiki\n";
#endif

  std::string data = json::write(json::object("project", project));

  CHECK(has<DEBUG>(options), printf("%s\n", data.c_str()));
  std::string body;
  result_t error = http::post("/projects.json", config, options,
                              http::status::CREATED, data, body);
  json::value root = json::read(body, false);
  if (error) {
    CHECK(json::TYPE_OBJECT != root.type(), return FAILURE);
    json::value *errors = root.object().get("errors");
    CHECK(!errors, return FAILURE);
    CHECK(json::TYPE_ARRAY != errors->type(), return FAILURE);
    for (json::value &error : errors->array()) {
      CHECK(json::TYPE_STRING != error.type(), return FAILURE);
      fprintf(stderr, "error: %s\n", error.string().c_str());
    }
  }

  // NOTE: Display result
  {
    printf("project created\n");
    CHECK(json::TYPE_OBJECT != root.type(), return FAILURE);

    auto project = root.object().get("project");
    CHECK(!project, return FAILURE);

    auto name = project->object().get("name");
    printf("name: %s\n", name->string().c_str());
    CHECK(!name, return FAILURE);

    auto id = project->object().get("id");
    CHECK(!id, return FAILURE);
    printf("id: %d\n", static_cast<int>(id->number()));

    auto identifier = project->object().get("identifier");
    CHECK(!identifier, return FAILURE);
    printf("identifier: %s\n", identifier->string().c_str());

    auto description = project->object().get("description");
    CHECK(!description, return FAILURE);
    printf("description: %s\n", description->string().c_str());

    auto homepage = project->object().get("homepage");
    CHECK(!homepage, return FAILURE);
    printf("homepage: %s\n", homepage->string().c_str());

    auto created_on = project->object().get("created_on");
    CHECK(!created_on, return FAILURE);
    printf("created on: %s\n", created_on->string().c_str());

    auto updated_on = project->object().get("updated_on");
    CHECK(!updated_on, return FAILURE);
    printf("updated on: %s\n", updated_on->string().c_str());
  }

  return SUCCESS;
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
  CHECK_RETURN(http::get(std::string("/projects/") + id + ".json", config,
                         options, body));

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
  CHECK_RETURN(http::get("/projects.json", config, options, body));

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
