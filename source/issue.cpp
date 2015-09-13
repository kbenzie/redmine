#include <http.h>
#include <issue.h>
#include <project.h>
#include <query.h>

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
  CHECK_RETURN(query::projects(config, options, projects));

  std::string filter;
  if (1 == argc) {
    std::string project_id;
    project_t *project = project::find(projects, argv[0]);

    CHECK(!project,
          fprintf(stderr, "invalid project id or identifier: %s\n", argv[0]);
          return FAILURE);

    filter = "?project_id=" + std::to_string(project->id);

    // TODO: Support listing all issues for a project

    std::vector<char> spaces(
        ((80 - project->name.size()) / 2) - project->name.size() / 2, ' ');
    spaces.back() = '\0';
    printf("%s%s issues\n", spaces.data(), project->name.c_str());
  }

  // TODO: Support listing other users issues

  std::vector<issue_t> issues;
  CHECK_RETURN(query::issues(filter, config, options, issues));

  printf(
      "    id | subject\n"
      "-------|----------------------------------------------------------------"
      "-------\n");
  for (auto &issue : issues) {
    printf("%6s | %s\n", issue.id.c_str(), issue.subject.c_str());
  }

  return SUCCESS;
}

result_t issue_new(int argc, char **argv, options_t options) {
  CHECK_MSG(0 == argc, "missing project id or identifier", return FAILURE);

  config_t config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::vector<project_t> projects;
  CHECK_RETURN(query::projects(config, options, projects));

  project_t *project = project::find(projects, argv[0]);
  CHECK(!project, fprintf(stderr, "invalid project: %s\n", argv[0]);
        return FAILURE);

  std::string subject;
  if (1 < argc) {
    CHECK(std::strcmp("-m", argv[1]),
          fprintf(stderr, "invliad option: %s\n", argv[1]);
          return FAILURE);
    CHECK(3 != argc, fprintf(stderr, "invalid argument: %s\n", argv[argc - 1]);
          return FAILURE);
    subject = argv[2];
  }

  std::vector<reference_t> trackers;
  CHECK_RETURN(query::trackers(config, options, trackers));

#if 0
  for (auto &tracker : trackers) {
    printf("%s\n", tracker.name.c_str());
  }
#endif

  std::vector<issue_status_t> statuses;
  CHECK_RETURN(query::issue_statuses(config, options, statuses));

#if 0
  for (auto &status : statuses) {
    printf("%s\n", status.name.c_str());
  }
#endif

  std::vector<enumeration_t> priorities;
  CHECK_RETURN(query::issue_priorities(config, options, priorities));

#if 0
  for (auto &priority : priorities) {
    printf("%s\n", priority.name.c_str());
  }
#endif

  static const char *fields[] = {
      "tracker_id",       "status_id",      "priority_id",     "category_id",
      "fixed_version_id", "assigned_to_id", "parent_issue_id", "custom_fields",
      "watcher_user_ids", "is_private",     "estimated_hours"};

  static const char *editor_fields[] = {"subject", "description"};

  for (auto field : fields) {
  }

  return SUCCESS;
}

void replace_all(std::string &str, const std::string &old_str,
                 const std::string &new_str) {
  size_t pos = 0;
  while ((pos = str.find(old_str, pos)) != std::string::npos) {
    str.replace(pos, old_str.length(), new_str);
    pos += new_str.length();
  }
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

  printf("%s: %s\n", I.id.c_str(), I.subject.c_str());
  printf("%s %s ", I.status.name.c_str(), I.tracker.name.c_str());
  printf("(%u %%) | ", I.done_ratio);
  printf("%s | ", I.priority.name.c_str());
  printf("%s (%u)\n", I.project.name.c_str(), I.project.id);
  printf("started: %s | ", I.start_date.c_str());
  if (!I.due_date.empty()) {
    printf("due: %s | ", I.due_date.c_str());
  }
  printf("created: %s | ", I.created_on.c_str());
  printf("updated: %s\n", I.updated_on.c_str());
  if (I.estimated_hours) {
    printf("estimated_hours: %u\n", I.estimated_hours);
  }
  printf("author: %s (%u) | ", I.author.name.c_str(), I.author.id);
  printf("assigned: %s (%u)\n", I.author.name.c_str(), I.author.id);
  if (!I.category.name.empty()) {
    printf("category: id: %u: name: %s\n", I.category.id,
           I.category.name.c_str());
  }
  std::string description = I.description;
  replace_all(description, "\\r\\n", "\n");
  printf("----------------\n%s\n", description.c_str());

  return SUCCESS;
}

result_t issue_edit(int argc, char **argv, options_t options) {
  fprintf(stderr, "unsupported: issue edit\n");
  return UNSUPPORTED;
}
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
    reference.id = id->number<uint32_t>();

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

result_t query::issues(std::string &filter, config_t &config, options_t options,
                       std::vector<issue_t> &out) {
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
    CHECK_RETURN(issue_deserialize(issue.object(), I));

    out.push_back(I);
  }

  return SUCCESS;
}

result_t query::issue_statuses(config_t &config, options_t options,
                               std::vector<issue_status_t> &statuses) {
  std::string body;
  CHECK_RETURN(http::get("/issue_statuses.json", config, options, body));

  auto root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);
  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto Statuses = root.object().get("issue_statuses");
  CHECK_JSON_PTR(Statuses, json::TYPE_ARRAY);

  for (auto &Status : Statuses->array()) {
    CHECK_JSON_TYPE(Status, json::TYPE_OBJECT);
    issue_status_t status;

    auto name = Status.object().get("name");
    CHECK_JSON_PTR(name, json::TYPE_STRING);
    status.name = name->string();

    auto id = Status.object().get("id");
    CHECK_JSON_PTR(id, json::TYPE_NUMBER);
    status.id = id->number<uint32_t>();

    auto is_default = Status.object().get("is_default");
    if (is_default) {
      CHECK_JSON_TYPE((*is_default), json::TYPE_BOOL);
      status.is_default = is_default->boolean();
    }

    auto is_closed = Status.object().get("is_closed");
    if (is_closed) {
      CHECK_JSON_TYPE((*is_closed), json::TYPE_BOOL);
      status.is_closed = is_closed->boolean();
    }

    statuses.push_back(status);
  }

  return SUCCESS;
}

result_t query::issue_priorities(config_t &config, options_t options,
                                 std::vector<enumeration_t> &priorities) {
  return query::enumerations("issue_priorities", config, options, priorities);
}
