// Copyright (C) 2015 Kenenth Benzie
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <enumeration.h>
#include <http.h>
#include <issue.h>
#include <project.h>
#include <membership.h>
#include <role.h>
#include <tracker.h>
#include <util.h>
#include <version.h>

#include <json/json.hpp>

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

redmine::issue::issue()
    : id(0),
      subject(),
      description(),
      start_date(),
      due_date(),
      created_on(),
      updated_on(),
      done_ratio(0),
      estimated_hours(0),
      project(),
      tracker(),
      status(),
      priority(),
      author(),
      assigned_to(),
      category() {}

static std::string &ltrim(std::string &str) {
  str.erase(str.begin(),
            std::find_if(str.begin(), str.end(),
                         std::not1(std::ptr_fun<int, int>(std::isspace))));

  return str;
}

static std::string &rtrim(std::string &str) {
  str.erase(std::find_if(str.rbegin(), str.rend(),
                         std::not1(std::ptr_fun<int, int>(std::isspace)))
                .base(),
            str.end());
  return str;
}

static std::string &trim(std::string &str) { return ltrim(rtrim(str)); }

redmine::result redmine::issue::init(const json::object &object) {
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
    CHECK_JSON_TYPE(*DueDate, json::TYPE_STRING);
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
    CHECK_JSON_TYPE(*EstimatedHours, json::TYPE_NUMBER);
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
  if (AssignedTo) {
    CHECK_JSON_TYPE((*AssignedTo), json::TYPE_OBJECT);
    CHECK_RETURN(getRef(AssignedTo->object(), assigned_to));
  }

  auto Category = object.get("category");
  if (Category) {
    CHECK_JSON_TYPE(*Category, json::TYPE_OBJECT);
    CHECK_RETURN(getRef(Category->object(), category));
  }

  return SUCCESS;
}

redmine::result redmine::issue::get(const uint32_t ID,
                                    const redmine::config &config,
                                    redmine::options &options) {
  return get(std::to_string(ID), config, options);
}

redmine::result redmine::issue::get(const std::string &ID,
                                    const redmine::config &config,
                                    redmine::options &options) {
  std::string body;
  CHECK_RETURN(http::get("/issues/" + ID + ".json?include=journals", config,
                         options, body));

  json::value Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);

  CHECK(options.debug, printf("%s\n", json::write(Root, "  ").c_str()));

  auto Issue = Root.object().get("issue");
  CHECK_JSON_PTR(Issue, json::TYPE_OBJECT);

  CHECK_RETURN(init(Issue->object()));

  return SUCCESS;
}

redmine::result redmine::action::issue(redmine::cl::args &args,
                                       redmine::config &config,
                                       redmine::current_user &user,
                                       redmine::options &options) {
  if (0 == args.count()) {
    fprintf(stderr,
            "usage: redmine issue <action> [args]\n"
            "actions:\n"
            "        list [project]\n"
            "        new <project> [-m <subject>]\n"
            "        show [-r] <id>\n"
            "        update <id>\n");
    return SUCCESS;
  }

  if (!strcmp("list", args[0])) {
    return issue_list(++args, config, options);
  }

  if (!strcmp("new", args[0])) {
    return issue_new(++args, config, user, options);
  }

  if (!strcmp("show", args[0])) {
    return issue_show(++args, config, options);
  }

  if (!strcmp("update", args[0])) {
    return issue_update(++args, config, user, options);
  }

  fprintf(stderr, "invalid argument: %s\n", args[0]);
  return FAILURE;
}

redmine::result redmine::action::issue_list(redmine::cl::args &args,
                                            redmine::config &config,
                                            redmine::options &options) {
  CHECK(args.count() > 1, fprintf(stderr, "invalid argument: %s\n", args[1]));

  std::vector<redmine::project> projects;
  CHECK_RETURN(query::projects(config, options, projects));

  std::string filter;
  if (1 == args.count()) {
    redmine::project *project = nullptr;
    for (auto &Project : projects) {
      if (Project == args[0]) {
        project = &Project;
        break;
      }
    }

    CHECK(!project,
          fprintf(stderr, "invalid project id or identifier: %s\n", args[0]);
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
static uint32_t get_answer_id(const std::string &name,
                              const std::vector<Info> &infos,
                              const bool allow_none) {
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
      if (allow_none) {
        if (answer.empty()) {
          return 0;
        }
      }
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

static uint32_t get_user_id(const char *name,
                            const std::vector<redmine::membership> &memberships,
                            const bool allow_none) {
  uint32_t assignee_id = 0;
  if (memberships.size()) {
    while (!assignee_id) {
      printf("%s (", name);
      for (size_t i = 0; i < memberships.size(); ++i) {
        printf("%s", memberships[i].user.name.c_str());
        if (i < memberships.size() - 1) {
          printf("|");
        }
      }
      printf("): ");
      std::string answer;
      std::getline(std::cin, answer);
      if (allow_none && answer.empty()) {
        return 0;
      }
      for (auto &membership : memberships) {
        if (answer == membership.user.name) {
          assignee_id = membership.user.id;
          break;
        }
      }
    }
  }
  return assignee_id;
}

redmine::result redmine::action::issue_new(redmine::cl::args &args,
                                           redmine::config &config,
                                           redmine::current_user &user,
                                           redmine::options &options) {
  CHECK_MSG(0 == args.count(), "missing project id or identifier",
            return FAILURE);

  std::vector<redmine::project> projects;
  CHECK_RETURN(query::projects(config, options, projects));

  redmine::project *project = nullptr;
  for (auto &Project : projects) {
    if (Project == args[0]) {
      project = &Project;
      break;
    }
  }
  CHECK(!project, fprintf(stderr, "invalid project: %s\n", args[0]);
        return FAILURE);

  std::string subject;
  if (1 < args.count()) {
    CHECK(std::strcmp("-m", args[1]),
          fprintf(stderr, "invalid option: %s\n", args[1]);
          return FAILURE);
    CHECK(3 != args.count(),
          fprintf(stderr, "invalid argument: %s\n", args.back());
          return FAILURE);
    subject = args[2];
  }

  std::vector<redmine::reference> trackers;
  CHECK_RETURN(query::trackers(config, options, trackers));

  std::vector<redmine::issue_status> statuses;
  CHECK_RETURN(query::issue_statuses(config, options, statuses));

  std::vector<redmine::enumeration> priorities;
  CHECK_RETURN(query::issue_priorities(config, options, priorities));

  std::vector<redmine::issue_category> issue_categories;
  if (user.permissions.manage_categories) {
    CHECK_RETURN(query::issue_categories(project->identifier, config, options,
                                         issue_categories));
  }

  std::vector<redmine::version> versions;
  CHECK_RETURN(query::versions(project->identifier, config, options, versions));

  std::vector<redmine::membership> memberships;
  CHECK_RETURN(
      query::memberships(project->identifier, config, options, memberships));

  // TODO: Parent Issue.
  // TODO: Custom Fields.
  // TODO: Is Private.
  // TODO: Estimated Hours.

  // TODO: Get permissions for this user & project.

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

  std::string command = config.editor + " " + filename;
  int result = std::system(command.c_str());
  CHECK(result,
        fprintf(stderr, "failed to load editor %s\n", config.editor.c_str());
        return FAILURE);

  std::string description;
  {
    std::ifstream file(filename);
    CHECK(!file.is_open(),
          fprintf(stderr, "Aborted due to empty update file.\n");
          return FAILURE);
    std::getline(file, subject);
    subject = trim(subject);
    CHECK(subject.empty(), fprintf(stderr, "Aborted due to empty subject.\n");
          return FAILURE);

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

    description = trim(description);
    CHECK(description.empty(),
          fprintf(stderr, "Aborted due to empty description.\n");
          return FAILURE);
  }

  // NOTE: Remove temoryary file.
  util::rm(filename);

  // NOTE: Ask for user input.
  uint32_t status_id = get_answer_id("Status", statuses, false);
  uint32_t tracker_id = get_answer_id("Tracker", trackers, false);
  uint32_t priority_id = get_answer_id("Priority", priorities, false);
  uint32_t category_id = 0;
  if (!issue_categories.empty()) {
    category_id = get_answer_id("Category", issue_categories, false);
  }
  uint32_t fixed_version_id = get_answer_id("Target Version", versions, false);
  uint32_t assigned_to_id = get_user_id("Assignee", memberships, true);
  json::array watcher_user_ids;
  while (uint32_t watcher_id = get_user_id("Watcher", memberships, true)) {
    watcher_user_ids.append(watcher_id);
  }

  // TODO: parent_issue
  // TODO: custom_fields
  // TODO: is_private
  // TODO: estimated_hours

  json::object issue;
  issue.add("project_id", project->id);
  issue.add("tracker_id", tracker_id);
  issue.add("status_id", status_id);
  issue.add("priority_id", priority_id);
  issue.add("subject", subject);
  issue.add("description", description);
  if (category_id) {
    issue.add("category_id", category_id);
  }
  if (fixed_version_id) {
    issue.add("fixed_version_id", fixed_version_id);
  }
  if (assigned_to_id) {
    issue.add("assigned_to_id", assigned_to_id);
  }
#if 0
  issue.add("parent_issue_id", parent_issue_id);
  issue.add("custom_fields", json::value());
#endif
  if (watcher_user_ids.size()) {
    issue.add("watcher_user_ids", watcher_user_ids);
  }
#if 0
  issue.add("is_private", is_private);
  issue.add("estimated_hours", estimated_hours);
#endif

  std::string data = json::write(json::object{"issue", issue}, "  ");

  CHECK(options.debug, printf("%s\n", data.c_str()));

  std::string body;
  CHECK_RETURN(http::post("/issues.json", config, options, http::code::CREATED,
                          data, body))

  auto ResponseRoot = json::read(body, false);
  CHECK_JSON_TYPE(ResponseRoot, json::TYPE_OBJECT);
  CHECK(options.debug, printf("%s\n", json::write(ResponseRoot, "  ").c_str()));

  // NOTE: Display new issue id and path to website.
  auto Issue = ResponseRoot.object().get("issue");
  CHECK_JSON_PTR(Issue, json::TYPE_OBJECT);

  auto Id = Issue->object().get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
  uint32_t id = Id->number<uint32_t>();

  printf(
      "created issue %u\n"
      "%s/issues/%u\n",
      id, config.current->url.c_str(), id);

  return SUCCESS;
}

redmine::result redmine::action::issue_show(redmine::cl::args &args,
                                            redmine::config &config,
                                            redmine::options &options) {
  CHECK(0 == args.count(), fprintf(stderr, "missing issue id\n");
        return FAILURE);
  CHECK(2 < args.count(), fprintf(stderr, "invalid argument: %s\n", args[1]);
        return FAILURE);

  bool raw = false;
  const char *id = nullptr;
  if (2 == args.count()) {
    if (!strcmp("-r", args[0])) {
      raw = true;
      id = args[1];
    } else {
      fprintf(stderr, "invalid option: %s\n", args[0]);
    }
  } else {
    id = args[0];
  }

  if (!raw && !config.browser.empty()) {
    std::string command =
        config.browser + " " + config.current->url + "/issues/" + args[0];
    int error = std::system(command.c_str());
    CHECK(error, fprintf(stderr, "failued to open browser %s\n",
                         config.browser.c_str());
          return FAILURE)
    return SUCCESS;
  }

  redmine::issue issue;
  CHECK_RETURN(issue.get(id, config, options));

  // TODO: Improve layout of issue details.
  printf("%u: %s\n", issue.id, issue.subject.c_str());
  printf("%s | %s ", issue.tracker.name.c_str(), issue.status.name.c_str());
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
  printf("----------------\n%s", description.c_str());
  if ('\n' != description.back()) {
    printf("\n");
  }

  // TODO: journals
  // TODO: watchers
  // TODO: children
  // TODO: attachments
  // TODO: relations
  // TODO: changesets
  // TODO: journals
  // TODO: watchers

  return SUCCESS;
}

redmine::result redmine::action::issue_update(redmine::cl::args &args,
                                              redmine::config &config,
                                              redmine::current_user &user,
                                              redmine::options &options) {
  CHECK(0 == args.count(), fprintf(stderr, "missing issue <id>\n");
        return FAILURE);
  CHECK(1 != args.count(), fprintf(stderr, "invalid argument: %s\n", args[1]);
        return FAILURE);

  // NOTE: Get the issue and check its valid.
  std::string id(args[0]);
  redmine::issue issue;
  CHECK_RETURN(issue.get(id, config, options));

  std::vector<redmine::issue_status> statuses;
  CHECK_RETURN(query::issue_statuses(config, options, statuses));

  std::vector<redmine::membership> memberships;
  CHECK_RETURN(query::memberships(std::to_string(issue.project.id), config,
                                  options, memberships));

  // TODO: Get adjustable properties.

  // TODO: Populate file?
  std::string filename("issue.redmine");

  std::string command = config.editor + " " + filename;
  CHECK(std::system(command.c_str()),
        fprintf(stderr, "fail to load editor %s\n", config.editor.c_str());
        return FAILURE);

  // NOTE: Read notes from temporary file.
  std::string notes;
  {
    std::ifstream file(filename);
    CHECK(!file.is_open(), fprintf(stderr, "Aborted due to empty note.\n");
          return FAILURE);
    notes.assign((std::istreambuf_iterator<char>(file)),
                 std::istreambuf_iterator<char>());
  }

  // TODO: Remove temoryary file.
  util::rm(filename);

  // TODO: Determine if notes only contain white space then empty the string.

  // TODO: Interactive mode option?

  uint32_t status_id = get_answer_id("Status", statuses, true);

  // NOTE: Use 101 to signify user is not updaing the done ration.
  uint32_t done_ratio = 101;
  while (true) {
    printf("Done %u%%: ", issue.done_ratio);
    std::string answer;
    std::getline(std::cin, answer);
    if (answer.empty()) {
      break;
    }
    char *end = nullptr;
    uint32_t done = std::strtoul(answer.c_str(), &end, 10);
    if (answer.data() + answer.size() == end) {
      if (0 != done % 5) {
        continue;
      }
      done_ratio = done;
      break;
    }
  }

  uint32_t assigned_to_id = get_user_id("Assignee", memberships, true);

  json::object Issue;
  if (!notes.empty()) {
    Issue.add("notes", notes);
  }
  if (101 != done_ratio) {
    Issue.add("done_ratio", done_ratio);
  }
  if (status_id) {
    Issue.add("status_id", status_id);
  }
  if (assigned_to_id) {
    Issue.add("assigned_to_id", assigned_to_id);
  }

  std::string json = json::write(json::object("issue", Issue), "  ");
  CHECK(options.debug, printf("%s\n", json.c_str()));

  CHECK_RETURN(http::put("/issues/" + id + ".json", config, options,
                         http::code::OK, json));

  printf(
      "updated issue %u\n"
      "%s/issues/%u\n",
      issue.id, config.current->url.c_str(), issue.id);

  return SUCCESS;
}

redmine::result redmine::query::issues(std::string &filter, config &config,
                                       redmine::options &options,
                                       std::vector<issue> &issues) {
  std::string body;
  CHECK_RETURN(http::get("/issues.json" + filter, config, options, body));

  auto Root = json::read(body, true);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);

  CHECK(options.debug, printf("%s\n", json::write(Root, "  ").c_str()));

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

redmine::result redmine::query::issue_statuses(
    redmine::config &config, redmine::options &options,
    std::vector<issue_status> &statuses) {
  std::string body;
  CHECK_RETURN(http::get("/issue_statuses.json?offset=0&limit=1000000", config,
                         options, body));

  auto root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);
  CHECK(options.debug, printf("%s\n", json::write(root, "  ").c_str()));

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
      CHECK_JSON_TYPE(*is_default, json::TYPE_BOOL);
      status.is_default = is_default->boolean();
    }

    auto is_closed = Status.object().get("is_closed");
    if (is_closed) {
      CHECK_JSON_TYPE(*is_closed, json::TYPE_BOOL);
      status.is_closed = is_closed->boolean();
    }

    statuses.push_back(status);
  }

  return SUCCESS;
}

redmine::result redmine::query::issue_categories(
    const std::string &project, redmine::config &config,
    redmine::options &options, std::vector<issue_category> &issue_categories) {
  std::string body;
  CHECK_RETURN(http::get(
      "/projects/" + project + "/issue_categories.json?offset=0&limit=1000000",
      config, options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(options.debug, printf("%s\n", json::write(Root, "  ").c_str()));

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
      CHECK_JSON_TYPE(*AssignedTo, json::TYPE_OBJECT);
      CHECK_RETURN(issue_category.assigned_to.init(AssignedTo->object()));
    }

    issue_categories.push_back(issue_category);
  }

  return SUCCESS;
}
