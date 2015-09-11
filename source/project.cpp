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
            "        list\n"
            "        new <name> <identifier>\n"
            "        show <show>\n");
    return FAILURE;
  }

  if (!strcmp("list", argv[0])) {
    return project_list(argc - 1, argv + 1, options);
  }

  if (!strcmp("new", argv[0])) {
    return project_new(argc - 1, argv + 1, options);
  }

  if (!strcmp("show", argv[0])) {
    return project_show(argc - 1, argv + 1, options);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result_t project_list(int argc, char **argv, options_t options) {
  CHECK(0 != argc, fprintf(stderr, "invalid argument: %s\n", argv[0]);
        return INVALID_ARGUMENT);

  // TODO: Support cached project list for command line completion.

  config_t config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::vector<project_t> projects;
  CHECK_RETURN(project_list_fetch(config, options, projects));

  // TODO: Display information about the output of the fields below?
  printf(
      "  id | identifier                        | name\n"
      "-----|-----------------------------------|------------------------------"
      "--------\n");
  for (auto &project : projects) {
    int32_t numSpaces = 34 - project.identifier.size();
    std::vector<char> spaces((0 > numSpaces) ? 0 : numSpaces, ' ');
    spaces.back() = '\0';
    printf("%4u | %s%s | %s\n", project.id, project.identifier.c_str(),
           spaces.data(), project.name.c_str());
  }

  return SUCCESS;
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

  std::string data = json::write(json::object("project", project));

  CHECK(has<DEBUG>(options), printf("%s\n", data.c_str()));
  std::string body;
  result_t error = http::post("/projects.json", config, options,
                              http::status::CREATED, data, body);
  json::value root = json::read(body, false);
  if (error) {
    CHECK_JSON_TYPE(root, json::TYPE_OBJECT);
    json::value *errors = root.object().get("errors");
    CHECK_JSON_PTR(errors, json::TYPE_ARRAY);
    for (json::value &error : errors->array()) {
      CHECK_JSON_TYPE(error, json::TYPE_STRING);
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
    printf("id: %u\n", static_cast<uint32_t>(id->number()));

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
  CHECK(1 < argc, fprintf(stderr, "invalid argument: %s\n", argv[1]);
        return FAILURE);

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
  project_t P;
  CHECK_RETURN(project_deserialize(project, P));

  printf("       name: %s\n", P.name.c_str());
  printf("         id: %u\n", P.id);
  printf(" identifier: %s\n", P.identifier.c_str());
  printf("description: %s\n", P.description.c_str());
  if (!P.homepage.empty()) {
    printf("   homepage: %s\n", P.homepage.c_str());
  }
  printf(" created_on: %s\n", P.created_on.c_str());
  printf(" updated_on: %s\n", P.updated_on.c_str());
  if (!P.parent.name.empty()) {
    printf("parent name: %s\n", P.parent.name.c_str());
    printf("  parent id: %u\n", P.parent.id);
  }

  return SUCCESS;
}
}

result_t project_deserialize(const json::object &project, project_t &out) {
  auto name = project.get("name");
  CHECK_JSON_PTR(name, json::TYPE_STRING);
  out.name = name->string();

  auto id = project.get("id");
  CHECK_JSON_PTR(id, json::TYPE_NUMBER);
  out.id = id->number<uint32_t>();

  auto identifier = project.get("identifier");
  CHECK_JSON_PTR(identifier, json::TYPE_STRING);
  out.identifier = identifier->string();

  auto description = project.get("description");
  CHECK_JSON_PTR(description, json::TYPE_STRING);
  out.description = description->string();

  auto created_on = project.get("created_on");
  CHECK_JSON_PTR(created_on, json::TYPE_STRING);
  out.created_on = created_on->string();

  auto updated_on = project.get("updated_on");
  CHECK_JSON_PTR(updated_on, json::TYPE_STRING);
  out.updated_on = updated_on->string();

  auto parent = project.get("parent");
  if (parent) {
    CHECK_JSON_TYPE((*parent), json::TYPE_OBJECT);

    auto name = parent->object().get("name");
    CHECK_JSON_PTR(name, json::TYPE_STRING);
    out.parent.name = name->string();

    auto id = parent->object().get("id");
    CHECK_JSON_PTR(id, json::TYPE_NUMBER);
    out.parent.id = id->number<uint32_t>();
  }

  return SUCCESS;
}

result_t project_list_fetch(config_t &config, options_t options,
                            std::vector<project_t> &out) {
  std::string body;
  CHECK_RETURN(http::get("/projects.json", config, options, body));

  auto root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto projects = root.object().get("projects");
  CHECK_JSON_PTR(projects, json::TYPE_ARRAY);

  for (auto project : projects->array()) {
    CHECK_JSON_TYPE(project, json::TYPE_OBJECT);

    project_t P;
    CHECK_RETURN(project_deserialize(project.object(), P));

    out.push_back(P);
  }

  return SUCCESS;
}
