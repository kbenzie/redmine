#include <enumeration.hpp>
#include <http.h>
#include <issue.h>
#include <project.h>
#include <tracker.hpp>

#include <json/json.hpp>

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace redmine {
issue::issue() {}

result issue::init(const json::object &object) {
  auto Id = object.get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
  id = Id->number<uint32_t>();

  auto Subject = object.get("subject");
  CHECK_JSON_PTR(Subject, json::TYPE_STRING);
  subject = Subject->string();

  auto Description = object.get("description");
  CHECK_JSON_PTR(Description, json::TYPE_STRING);
  description = Description->string();

  auto StartDate = object.get("start_date");
  CHECK_JSON_PTR(StartDate, json::TYPE_STRING);
  start_date = StartDate->string();

  auto DueDate = object.get("due_date");
  if (DueDate) {
    CHECK_JSON_TYPE((*DueDate), json::TYPE_STRING);
    due_date = DueDate->string();
  }

  auto CreatedOn = object.get("created_on");
  CHECK_JSON_PTR(CreatedOn, json::TYPE_STRING);
  created_on = CreatedOn->string();

  auto UpdatedOn = object.get("updated_on");
  CHECK_JSON_PTR(UpdatedOn, json::TYPE_STRING);
  updated_on = UpdatedOn->string();

  auto DoneRatio = object.get("done_ratio");
  CHECK_JSON_PTR(DoneRatio, json::TYPE_NUMBER);
  done_ratio = DoneRatio->number<uint32_t>();

  auto EstimatedHours = object.get("estimated_hours");
  if (EstimatedHours) {
    CHECK_JSON_TYPE((*EstimatedHours), json::TYPE_NUMBER);
    estimated_hours = EstimatedHours->number<uint32_t>();
  }

  auto getRef = [](const json::object &object, reference &reference) -> result {
    auto name = object.get("name");
    CHECK_JSON_PTR(name, json::TYPE_STRING);
    reference.name = name->string();

    auto id = object.get("id");
    CHECK_JSON_PTR(id, json::TYPE_NUMBER);
    reference.id = id->number<uint32_t>();

    return SUCCESS;
  };

  auto Project = object.get("project");
  CHECK_JSON_PTR(Project, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(Project->object(), project));

  auto Tracker = object.get("tracker");
  CHECK_JSON_PTR(Tracker, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(Tracker->object(), tracker));

  auto Status = object.get("status");
  CHECK_JSON_PTR(Status, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(Status->object(), status));

  auto Priority = object.get("priority");
  CHECK_JSON_PTR(Priority, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(Priority->object(), priority));

  auto Author = object.get("author");
  CHECK_JSON_PTR(Author, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(Author->object(), author));

  auto AssignedTo = object.get("assigned_to");
  CHECK_JSON_PTR(AssignedTo, json::TYPE_OBJECT);
  CHECK_RETURN(getRef(AssignedTo->object(), assigned_to));

  auto Category = object.get("category");
  if (Category) {
    CHECK_JSON_TYPE((*Category), json::TYPE_OBJECT);
    CHECK_RETURN(getRef(Category->object(), category));
  }

  return SUCCESS;
}

namespace action {
result issue(int argc, char **argv, options options) {
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

result issue_list(int argc, char **argv, options options) {
  CHECK(argc > 1, fprintf(stderr, "invalid argument: %s\n", argv[1]));

  redmine::config config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::vector<redmine::project> projects;
  CHECK_RETURN(query::projects(config, options, projects));

  std::string filter;
  if (1 == argc) {
    redmine::project *project = nullptr;
    for (auto &Project : projects) {
      if (Project == argv[0]) {
        project = &Project;
        break;
      }
    }

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

  std::vector<redmine::issue> issues;
  CHECK_RETURN(query::issues(filter, config, options, issues));

  printf(
      "    id | subject\n"
      "-------|----------------------------------------------------------------"
      "-------\n");
  for (auto &issue : issues) {
    printf("%6d | %s\n", issue.id, issue.subject.c_str());
  }

  return SUCCESS;
}

result issue_new(int argc, char **argv, options options) {
  CHECK_MSG(0 == argc, "missing project id or identifier", return FAILURE);

  redmine::config config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::vector<redmine::project> projects;
  CHECK_RETURN(query::projects(config, options, projects));

  redmine::project *project = nullptr;
  for (auto &Project : projects) {
    if (Project == argv[0]) {
      project = &Project;
      break;
    }
  }
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

  std::vector<reference> trackers;
  CHECK_RETURN(query::trackers(config, options, trackers));

#if 0
    for (auto &tracker : trackers) {
      printf("%s\n", tracker.name.c_str());
    }
#endif

  std::vector<issue_status> issue_statuses;
  CHECK_RETURN(query::issue_statuses(config, options, issue_statuses));

#if 0
    for (auto &status : statuses) {
      printf("%s\n", status.name.c_str());
    }
#endif

  std::vector<redmine::enumeration> priorities;
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

result issue_show(int argc, char **argv, options options) {
  CHECK(0 == argc, fprintf(stderr, "missing issue id\n"); return FAILURE);
  CHECK(1 < argc, fprintf(stderr, "invalid argument: %s\n", argv[1]);
        return FAILURE);

  redmine::config config;
  CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
        return INVALID_CONFIG);

  std::string id(argv[0]);

  std::string body;
  CHECK_RETURN(
      http::get(std::string("/issues/") + id + ".json", config, options, body));

  json::value Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(Root, "  ").c_str()));

  auto Issue = Root.object().get("issue");
  CHECK_JSON_PTR(Issue, json::TYPE_OBJECT);

  redmine::issue issue;
  CHECK_RETURN(issue.init(Issue->object()));

  printf("%u: %s\n", issue.id, issue.subject.c_str());
  printf("%s %s ", issue.status.name.c_str(), issue.tracker.name.c_str());
  printf("(%u %%) | ", issue.done_ratio);
  printf("%s | ", issue.priority.name.c_str());
  printf("%s (%u)\n", issue.project.name.c_str(), issue.project.id);
  printf("started: %s | ", issue.start_date.c_str());
  if (!issue.due_date.empty()) {
    printf("due: %s | ", issue.due_date.c_str());
  }
  printf("created: %s | ", issue.created_on.c_str());
  printf("updated: %s\n", issue.updated_on.c_str());
  if (issue.estimated_hours) {
    printf("estimated_hours: %u\n", issue.estimated_hours);
  }
  printf("author: %s (%u) | ", issue.author.name.c_str(), issue.author.id);
  printf("assigned: %s (%u)\n", issue.author.name.c_str(), issue.author.id);
  if (!issue.category.name.empty()) {
    printf("category: id: %u: name: %s\n", issue.category.id,
           issue.category.name.c_str());
  }
  std::string description = issue.description;
  replace_all(description, "\\r\\n", "\n");
  printf("----------------\n%s\n", description.c_str());

  return SUCCESS;
}

result issue_edit(int argc, char **argv, options options) {
  fprintf(stderr, "unsupported: issue edit\n");
  return UNSUPPORTED;
}
}

result issue_serialize(const issue &issue, json::object &out) {
  fprintf(stderr, "issue_serialize not implemented!\n");
  return UNSUPPORTED;
}

result query::issues(std::string &filter, config &config, options options,
                     std::vector<issue> &issues) {
  std::string body;
  CHECK_RETURN(http::get("/issues.json" + filter, config, options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);

  CHECK(has<DEBUG>(options), printf("%s\n", json::write(Root, "  ").c_str()));

  auto Issues = Root.object().get("issues");
  CHECK_JSON_PTR(Issues, json::TYPE_ARRAY);

  for (auto Issue : Issues->array()) {
    CHECK_JSON_TYPE(Issue, json::TYPE_OBJECT);

    redmine::issue issue;
    CHECK_RETURN(issue.init(Issue.object()));

    issues.push_back(issue);
  }

  return SUCCESS;
}

result query::issue_statuses(config &config, options options,
                               std::vector<issue_status> &statuses) {
  std::string body;
  CHECK_RETURN(http::get("/issue_statuses.json", config, options, body));

  auto root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);
  CHECK(has<DEBUG>(options), printf("%s\n", json::write(root, "  ").c_str()));

  auto Statuses = root.object().get("issue_statuses");
  CHECK_JSON_PTR(Statuses, json::TYPE_ARRAY);

  for (auto &Status : Statuses->array()) {
    CHECK_JSON_TYPE(Status, json::TYPE_OBJECT);
    issue_status status;

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
}
