#include <http.h>
#include <issue.h>
#include <project.h>

#include <json/json.hpp>

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace action {
result_t issue(int argc, char **argv, options_t options) {
  if (0 == argc) {
    fprintf(stderr,
            "usage: redmine issue <action> [args]\n"
            "actions:\n"
            "        list [project]\n"
            "        new <project> [-m <subject>]\n"
            "        show <id>\n"
            "        edit <id>\n");
    return FAILURE;
  }

  if (!strcmp("list", argv[0])) {
    return issue_list(argc - 1, argv + 1, options);
  }

  if (!strcmp("new", argv[0])) {
    return issue_new(argc - 1, argv + 1, options);
  }

  if (!strcmp("show", argv[0])) {
    return issue_show(argc - 1, argv + 1, options);
  }

  if (!strcmp("edit", argv[0])) {
    return issue_edit(argc - 1, argv + 1, options);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result_t issue_list(int argc, char **argv, options_t options) {
  CHECK(argc > 1, fprintf(stderr, "invalid argument: %s\n", argv[1]));

  config_t config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::vector<project_t> projects;
  CHECK_RETURN(project_list_fetch(config, options, projects));

  std::string filter;
  if (1 == argc) {
    std::string project_id;
    for (auto &project : projects) {
      if (project.name == argv[0] || project.id == argv[0] ||
          project.identifier == argv[0]) {
        project_id = project.id;
        break;
      }
    }

    CHECK(project_id.empty(),
          fprintf(stderr, "invalid argument: %s\n", argv[0]);
          return FAILURE);

    filter = "?project_id=" + project_id;

    // TODO: Support listing all issues for a project
  }

  // TODO: Support listing other users issues

  std::vector<issue_t> issues;
  CHECK_RETURN(issue_list_fetch(filter, config, options, issues));

  printf(
      "id: subject\n"
      "--: -------\n");
  for (auto &issue : issues) {
    printf("%s: %s\n", issue.id.c_str(), issue.subject.c_str());
  }

  return SUCCESS;
}

result_t issue_new(int argc, char **argv, options_t options) {
  fprintf(stderr, "unsupported: issue new\n");
  return UNSUPPORTED;
}

result_t issue_show(int argc, char **argv, options_t options) {
  CHECK(0 == argc, fprintf(stderr, "missing issue id\n"); return FAILURE);
  CHECK(1 < argc, fprintf(stderr, "invalid argument: %s\n", argv[1]);
        return FAILURE);

  config_t config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::string id(argv[0]);

  std::string body;
  CHECK_RETURN(
      http::get(std::string("/issues/") + id + ".json", config, options, body));

  json::value root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto issue = root.object().get("issue");
  CHECK_JSON_PTR(issue, json::TYPE_OBJECT);

  issue_t I;
  CHECK_RETURN(issue_deserialize(issue->object(), I));

  issue_print(I);

  return SUCCESS;
}

result_t issue_edit(int argc, char **argv, options_t options) {
  fprintf(stderr, "unsupported: issue edit\n");
  return UNSUPPORTED;
}
}

void replace_all(std::string &str, const std::string &old_str,
                 const std::string &new_str) {
  size_t pos = 0;
  while ((pos = str.find(old_str, pos)) != std::string::npos) {
    str.replace(pos, old_str.length(), new_str);
    pos += new_str.length();
  }
}

void issue_print(const issue_t &issue) {
  printf("%s: %s\n", issue.id.c_str(), issue.subject.c_str());
  printf("%s %s ", issue.status.name.c_str(), issue.tracker.name.c_str());
  printf("(%u %%) | ", issue.done_ratio);
  printf("%s | ", issue.priority.name.c_str());
  printf("%s (%s)\n", issue.project.name.c_str(), issue.project.id.c_str());
  printf("started: %s | ", issue.start_date.c_str());
  if (!issue.due_date.empty()) {
    printf("due: %s | ", issue.due_date.c_str());
  }
  printf("created: %s | ", issue.created_on.c_str());
  printf("updated: %s\n", issue.updated_on.c_str());
  if (issue.estimated_hours) {
    printf("estimated_hours: %u\n", issue.estimated_hours);
  }
  printf("author: %s (%s) | ", issue.author.name.c_str(),
         issue.author.id.c_str());
  printf("assigned: %s (%s)\n", issue.author.name.c_str(),
         issue.author.id.c_str());
  if (!issue.category.name.empty()) {
    printf("category: id: %s: name: %s\n", issue.category.id.c_str(),
           issue.category.name.c_str());
  }
  std::string description = issue.description;
  replace_all(description, "\\r\\n", "\n");
  printf("----------------\n%s\n", description.c_str());
}

result_t issue_serialize(const issue_t &issue, json::object &out) {
  fprintf(stderr, "issue_serialize not implemented!\n");
  return UNSUPPORTED;
}

result_t issue_deserialize(const json::object &issue, issue_t &out) {
  auto id = issue.get("id");
  CHECK_JSON_PTR(id, json::TYPE_NUMBER);
  out.id = std::to_string(id->number<uint32_t>());

  auto subject = issue.get("subject");
  CHECK_JSON_PTR(subject, json::TYPE_STRING);
  out.subject = subject->string();

  auto description = issue.get("description");
  CHECK_JSON_PTR(description, json::TYPE_STRING);
  out.description = description->string();

  auto start_date = issue.get("start_date");
  CHECK_JSON_PTR(start_date, json::TYPE_STRING);
  out.start_date = start_date->string();

  auto due_date = issue.get("due_date");
  if (due_date) {
    CHECK_JSON_TYPE((*due_date), json::TYPE_STRING);
    out.due_date = due_date->string();
  }

  auto created_on = issue.get("created_on");
  CHECK_JSON_PTR(created_on, json::TYPE_STRING);
  out.created_on = created_on->string();

  auto updated_on = issue.get("updated_on");
  CHECK_JSON_PTR(updated_on, json::TYPE_STRING);
  out.updated_on = updated_on->string();

  auto done_ratio = issue.get("done_ratio");
  CHECK_JSON_PTR(done_ratio, json::TYPE_NUMBER);
  out.done_ratio = done_ratio->number<uint32_t>();

  auto estimated_hours = issue.get("estimated_hours");
  if (estimated_hours) {
    CHECK_JSON_TYPE((*estimated_hours), json::TYPE_NUMBER);
    out.estimated_hours = estimated_hours->number<uint32_t>();
  }

  auto getRef =
      [](const json::object &object, reference_t &reference) -> result_t {
    auto name = object.get("name");
    CHECK_JSON_PTR(name, json::TYPE_STRING);
    reference.name = name->string();

    auto id = object.get("id");
    CHECK_JSON_PTR(id, json::TYPE_NUMBER);
    reference.id = std::to_string(id->number<uint32_t>());

    return SUCCESS;
  };

  auto project = issue.get("project");
  CHECK_JSON_PTR(project, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(project->object(), out.project));

  auto tracker = issue.get("tracker");
  CHECK_JSON_PTR(tracker, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(tracker->object(), out.tracker));

  auto status = issue.get("status");
  CHECK_JSON_PTR(status, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(status->object(), out.status));

  auto priority = issue.get("priority");
  CHECK_JSON_PTR(priority, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(priority->object(), out.priority));

  auto author = issue.get("author");
  CHECK_JSON_PTR(author, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(author->object(), out.author));

  auto assigned_to = issue.get("assigned_to");
  CHECK_JSON_PTR(assigned_to, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(assigned_to->object(), out.assigned_to));

  auto category = issue.get("category");
  if (category) {
    CHECK_JSON_TYPE((*category), json::TYPE_OBJECT);
    CHECK_RETURN(getRef(category->object(), out.category));
  }

  return SUCCESS;
}

result_t issue_list_fetch(std::string &filter, config_t &config,
                          options_t options, std::vector<issue_t> &out) {
  std::string body;
  CHECK_RETURN(http::get("/issues.json" + filter, config, options, body));

  auto root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto issues = root.object().get("issues");
  CHECK_JSON_PTR(issues, json::TYPE_ARRAY);

  for (auto issue : issues->array()) {
    CHECK_JSON_TYPE(issue, json::TYPE_OBJECT);

    issue_t I;
    issue_deserialize(issue.object(), I);

    out.push_back(I);
  }

  return SUCCESS;
}
