#include <enumeration.hpp>
#include <http.h>
#include <issue.h>
#include <project.h>
#include <project_membership.hpp>
#include <tracker.hpp>
#include <util.h>
#include <version.hpp>

#include <json/json.hpp>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

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
  CHECK(config.load(), fprintf(stderr, "invalid config file\n");
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

template <typename Info>
uint32_t get_answer_id(const std::string &name, const std::vector<Info> &infos){
  uint32_t id = 0;
  if (infos.size()) {
    while (!id) {
      printf("%s (", name.c_str());
      for (size_t i = 0; i < infos.size(); ++i) {
        printf("%s", infos[i].name.c_str());
        if (i < infos.size() - 1) {
          printf("|");
        }
      }
      printf("): ");
      std::string answer;
      std::getline(std::cin, answer);
      printf("%s\n", answer.c_str());
      for (auto &info : infos) {
        if (answer == info.name) {
          id = info.id;
          break;
        }
      }
    }
  }
  return id;
}

void replace_all(std::string &str, const std::string &old_str,
                 const std::string &new_str) {
  size_t pos = 0;
  while ((pos = str.find(old_str, pos)) != std::string::npos) {
    str.replace(pos, old_str.length(), new_str);
    pos += new_str.length();
  }
}

result issue_new(int argc, char **argv, options options) {
  CHECK_MSG(0 == argc, "missing project id or identifier", return FAILURE);

  redmine::config config;
  CHECK(config.load(), fprintf(stderr, "invalid config file\n");
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

  std::vector<redmine::reference> trackers;
  CHECK_RETURN(query::trackers(config, options, trackers));

  std::vector<redmine::issue_status> issue_statuses;
  CHECK_RETURN(query::issue_statuses(config, options, issue_statuses));

  std::vector<redmine::enumeration> priorities;
  CHECK_RETURN(query::issue_priorities(config, options, priorities));

  std::vector<redmine::issue_category> issue_categories;
  CHECK_RETURN(query::issue_categories(project->identifier, config, options,
                                       issue_categories));

  std::vector<redmine::version> versions;
  CHECK_RETURN(query::versions(project->identifier, config, options, versions));

  std::vector<redmine::project_membership> memberships;
  CHECK_RETURN(query::project_memberships(project->identifier, config, options,
                                          memberships));

  // TODO: Parent Issue.
  // TODO: Custom Fields.
  // TODO: Is Private.
  // TODO: Estimated Hours.

  // NOTE: Open editor optionally populated with subject.
  std::string filename = util::getcwd();
  filename += "/issue.redmine";
  {
    // TODO: Don't overwrite existing file contents.
    std::ofstream file(filename);
    CHECK(!file.is_open(),
          fprintf(stderr, "could not create temporary file: %s\n",
                  filename.c_str());
          return FAILURE);
    if (!subject.empty()) {
      file << subject;
    }
    file << "\n----------------------------------------------------------------"
            "----------------\n\n";
  }

  // TODO: Get editor from redmine::config.
  std::string editor("vim");

  std::string command = editor + " " + filename;
  int result = std::system(command.c_str());
  CHECK_MSG(result, "editor exited with failure!", return FAILURE);

  std::string description;
  {
    std::ifstream file(filename);
    CHECK(!file.is_open(),
          fprintf(stderr, "could not open temporary file: %s\n",
                  filename.c_str());
          return FAILURE);
    std::getline(file, subject);
    std::string line;

    std::getline(file, line);
    CHECK(line !=
              "----------------------------------------------------------------"
              "----------------",
          fprintf(stderr, "invalid separator in %s: 2\n", filename.c_str());
          return FAILURE);

    while (file) {
      std::getline(file, line);
      description += line + "\n";
    }
  }

  // NOTE: Remove temoryary file.
  util::rm(filename);

  // NOTE: Ask for user input.
  uint32_t tracker_id = get_answer_id("Tracker", trackers);
  uint32_t status_id = get_answer_id("Status", issue_statuses);
  uint32_t priority_id = get_answer_id("Priority", priorities);
  uint32_t category_id = get_answer_id("Category", issue_categories);
  uint32_t fixed_version_id = get_answer_id("Target Version", versions);

  uint32_t assigned_to_id = 0;
  if (memberships.size()) {
    while (!assigned_to_id) {
      printf("Assignee (");
      for (size_t i = 0; i < memberships.size(); ++i) {
        printf("%s", memberships[i].user.name.c_str());
        if (i < memberships.size() - 1) {
          printf("|");
        }
      }
      printf("): ");
      std::string answer;
      std::getline(std::cin, answer);
      for (auto &membership : memberships) {
        if (answer == membership.user.name) {
          assigned_to_id = membership.user.id;
          break;
        }
      }
    }
  }

  // TODO: watcher_user_ids
  // TODO: parent_issue
  // TODO: custom_fields
  // TODO: is_private
  // TODO: estimated_hours

  replace_all(description, "\n", "\\n");

  json::object issue;
  issue.add("project_id", json::value(project->id));
  issue.add("tracker_id", json::value(tracker_id));
  issue.add("status_id", json::value(status_id));
  issue.add("priority_id", json::value(priority_id));
  issue.add("subject", json::value(subject));
  issue.add("description", json::value(description));
  if (category_id) {
    issue.add("category_id", json::value(category_id));
  }
  if (fixed_version_id) {
    issue.add("fixed_version_id", json::value(fixed_version_id));
  }
  issue.add("assigned_to_id", json::value(assigned_to_id));
#if 0
  issue.add("parent_issue_id", json::value());
  issue.add("custom_fields", json::value());
  issue.add("watcher_user_ids", json::value());
  issue.add("is_private", json::value());
  issue.add("estimated_hours", json::value());
#endif

  std::string data =
      json::write(json::value{json::object{"issue", issue}}, "  ");

  CHECK(has<DEBUG>(options), printf("%s\n", data.c_str()));

  std::string body;
  CHECK_RETURN(http::post("/issues.json", config, options, http::code::CREATED,
                          data, body))

  auto ResponseRoot = json::read(body, false);
  CHECK_JSON_TYPE(ResponseRoot, json::TYPE_OBJECT);
  CHECK(has<DEBUG>(options),
        printf("%s\n", json::write(ResponseRoot, "  ").c_str()));

  // NOTE: Display new issue id and path to website.
  auto Issue = ResponseRoot.object().get("issue");
  CHECK_JSON_PTR(Issue, json::TYPE_OBJECT);

  auto Id = Issue->object().get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
  uint32_t id = Id->number<uint32_t>();

  printf(
      "created issue %u\n"
      "%s/issues/%u\n",
      id, config.url.c_str(), id);

  return SUCCESS;
}

result issue_show(int argc, char **argv, options options) {
  CHECK(0 == argc, fprintf(stderr, "missing issue id\n"); return FAILURE);
  CHECK(1 < argc, fprintf(stderr, "invalid argument: %s\n", argv[1]);
        return FAILURE);

  redmine::config config;
  CHECK(config.load(), fprintf(stderr, "invalid config file\n");
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

result query::issue_categories(const std::string &project,
                               redmine::config &config, options options,
                               std::vector<issue_category> &issue_categories) {
  std::string body;
  CHECK_RETURN(http::get("/projects/" + project + "/issue_categories.json",
                         config, options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(has<DEBUG>(options), printf("%s\n", json::write(Root, "  ").c_str()));

  auto IssueCategories = Root.object().get("issue_categories");
  CHECK_JSON_PTR(IssueCategories, json::TYPE_ARRAY);

  for (auto &IssueCategory : IssueCategories->array()) {
    CHECK_JSON_TYPE(IssueCategory, json::TYPE_OBJECT);

    redmine::issue_category issue_category;

    auto Id = IssueCategory.object().get("id");
    CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
    issue_category.id = Id->number<uint32_t>();

    auto Name = IssueCategory.object().get("name");
    CHECK_JSON_PTR(Name, json::TYPE_STRING);
    issue_category.name = Name->string();

    auto Project = IssueCategory.object().get("project");
    CHECK_JSON_PTR(Project, json::TYPE_OBJECT);
    CHECK_RETURN(issue_category.project.init(Project->object()));

    auto AssignedTo = IssueCategory.object().get("assigned_to");
    if (AssignedTo) {
      CHECK_JSON_TYPE((*AssignedTo), json::TYPE_OBJECT);
      CHECK_RETURN(issue_category.assigned_to.init(AssignedTo->object()));
    }

    issue_categories.push_back(issue_category);
  }

  return SUCCESS;
}
}
