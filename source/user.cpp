#include <http.h>
#include <user.h>

#include <json/json.hpp>

#include <cstring>

namespace redmine {
user::user() {}

result user::init(const json::object &object) {
  auto Firstname = object.get("firstname");
  CHECK_JSON_PTR(Firstname, json::TYPE_STRING);
  firstname = Firstname->string();

  auto Lastname = object.get("lastname");
  CHECK_JSON_PTR(Lastname, json::TYPE_STRING);
  lastname = Lastname->string();

  name = firstname + " " + lastname;

  auto Id = object.get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
  id = Id->number<uint32_t>();

  auto Mail = object.get("mail");
  CHECK_JSON_PTR(Mail, json::TYPE_STRING);
  mail = Mail->string();

  auto Login = object.get("login");
  CHECK_JSON_PTR(Login, json::TYPE_STRING);
  login = Login->string();

  auto ApiKey = object.get("api_key");
  if (ApiKey) {
    CHECK_JSON_TYPE((*ApiKey), json::TYPE_STRING);
    api_key = ApiKey->string();
  }

  auto CreatedOn = object.get("created_on");
  CHECK_JSON_PTR(CreatedOn, json::TYPE_STRING);
  created_on = CreatedOn->string();

  auto LastLoginOn = object.get("last_login_on");
  CHECK_JSON_PTR(LastLoginOn, json::TYPE_STRING);
  last_login_on = LastLoginOn->string();

  auto Status = object.get("status");
  if (Status) {
    CHECK_JSON_PTR(Status, json::TYPE_NUMBER);
    status = Status->number<uint32_t>();
  }

  return SUCCESS;
}

current_user::current_user()
    : id(),
      login(),
      firstname(),
      lastname(),
      mail(),
      created_on(),
      last_login_on(),
      memberships(),
      project_permissions(),
      permissions() {}

result current_user::get(redmine::config &config, redmine::options options) {
  std::string body;
  CHECK_RETURN(http::get("/users/current.json?include=memberships,groups",
                         config, redmine::NONE, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(HAS_OPTION(DEBUG), printf("%s\n", json::write(Root, "  ").c_str()));

  auto User = Root.object().get("user");
  CHECK_JSON_PTR(User, json::TYPE_OBJECT);

  auto Id = User->object().get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
  id = Id->number<uint32_t>();

  auto Firstname = User->object().get("firstname");
  CHECK_JSON_PTR(Firstname, json::TYPE_STRING);
  firstname = Firstname->string();

  auto Lastname = User->object().get("lastname");
  CHECK_JSON_PTR(Lastname, json::TYPE_STRING);
  lastname = Lastname->string();

  auto Mail = User->object().get("mail");
  CHECK_JSON_PTR(Mail, json::TYPE_STRING);
  mail = Mail->string();

  auto CreateOn = User->object().get("created_on");
  CHECK_JSON_PTR(CreateOn, json::TYPE_STRING);
  created_on = CreateOn->string();

  auto LastLoginOn = User->object().get("last_login_on");
  CHECK_JSON_PTR(LastLoginOn, json::TYPE_STRING);
  last_login_on = LastLoginOn->string();

  auto Memberships = User->object().get("memberships");
  CHECK_JSON_PTR(Memberships, json::TYPE_ARRAY);
  for (auto &Membership : Memberships->array()) {
    CHECK_JSON_TYPE(Membership, json::TYPE_OBJECT);

    redmine::current_user::membership membership;

    auto Project = Membership.object().get("project");
    CHECK_JSON_PTR(Project, json::TYPE_OBJECT);
    CHECK_RETURN(membership.project.init(Project->object()));

    auto Roles = Membership.object().get("roles");
    CHECK_JSON_PTR(Roles, json::TYPE_ARRAY);
    for (auto &Role : Roles->array()) {
      CHECK_JSON_TYPE(Role, json::TYPE_OBJECT);

      redmine::reference role;
      CHECK_RETURN(role.init(Role.object()));

      membership.roles.push_back(role);
    }

    memberships.push_back(membership);
  }

  for (auto &membership : memberships) {
    redmine::permissions membership_permissions;
    for (auto &role : membership.roles) {
      redmine::permissions role_permissons;
      CHECK_RETURN(role_permissons.get(role.id, config, options));
      membership_permissions |= role_permissons;
      permissions |= role_permissons;
    }
    project_permissions[membership.project.id] = membership_permissions;
  }

  return SUCCESS;
}

bool redmine::current_user::can(redmine::permisson permisson) {
  switch (permisson) {
    case USE_PROJECT:
      return permissions.use_project;
    case USE_ISSUE:
      return permissions.use_issue;
    case USE_TIME_TRACKING:
      return permissions.use_time_tracking;
    case USE_NEWS:
      return permissions.use_news;
    case USE_DOCUMENT:
      return permissions.use_document;
    case USE_FILE:
      return permissions.use_file;
    case USE_WIKI:
      return permissions.use_wiki;
    case USE_REPOSITORY:
      return permissions.use_repository;
    case USE_FORUM:
      return permissions.use_forum;
    case USE_CALENDAR:
      return permissions.use_calendar;
    case USE_GANTT:
      return permissions.use_gantt;
    case ADD_PROJECT:
      return permissions.add_project;
    case EDIT_PROJECT:
      return permissions.edit_project;
    case CLOSE_PROJECT:
      return permissions.close_project;
    case SELECT_PROJECT_MODULES:
      return permissions.select_project_modules;
    case MANAGE_MEMBERS:
      return permissions.manage_members;
    case MANAGE_VERSIONS:
      return permissions.manage_versions;
    case ADD_SUBPROJECTS:
      return permissions.add_subprojects;
    case MANAGE_CATEGORIES:
      return permissions.manage_categories;
    case VIEW_ISSUES:
      return permissions.view_issues;
    case ADD_ISSUES:
      return permissions.add_issues;
    case EDIT_ISSUES:
      return permissions.edit_issues;
    case MANAGE_ISSUE_RELATIONS:
      return permissions.manage_issue_relations;
    case MANAGE_SUBTASKS:
      return permissions.manage_subtasks;
    case SET_ISSUES_PRIVATE:
      return permissions.set_issues_private;
    case SET_OWN_ISSUES_PRIVATE:
      return permissions.set_own_issues_private;
    case ADD_ISSUE_NOTES:
      return permissions.add_issue_notes;
    case EDIT_ISSUE_NOTES:
      return permissions.edit_issue_notes;
    case EDIT_OWN_ISSUE_NOTES:
      return permissions.edit_own_issue_notes;
    case VIEW_PRIVATE_NOTES:
      return permissions.view_private_notes;
    case SET_NOTES_PRIVATE:
      return permissions.set_notes_private;
    case MOVE_ISSUES:
      return permissions.move_issues;
    case DELETE_ISSUES:
      return permissions.delete_issues;
    case MANAGE_PUBLIC_QUERIES:
      return permissions.manage_public_queries;
    case SAVE_QUERIES:
      return permissions.save_queries;
    case VIEW_ISSUE_WATCHERS:
      return permissions.view_issue_watchers;
    case ADD_ISSUE_WATCHERS:
      return permissions.add_issue_watchers;
    case DELETE_ISSUE_WATCHERS:
      return permissions.delete_issue_watchers;
    case LOG_TIME:
      return permissions.log_time;
    case VIEW_TIME_ENTRIES:
      return permissions.view_time_entries;
    case EDIT_TIME_ENTRIES:
      return permissions.edit_time_entries;
    case EDIT_OWN_TIME_ENTRIES:
      return permissions.edit_own_time_entries;
    case MANAGE_PROJECT_ACTIVITIES:
      return permissions.manage_project_activities;
    case MANAGE_NEWS:
      return permissions.manage_news;
    case COMMENT_NEWS:
      return permissions.comment_news;
    case ADD_DOCUMENTS:
      return permissions.add_documents;
    case EDIT_DOCUMENTS:
      return permissions.edit_documents;
    case DELETE_DOCUMENTS:
      return permissions.delete_documents;
    case VIEW_DOCUMENTS:
      return permissions.view_documents;
    case MANAGE_FILES:
      return permissions.manage_files;
    case VIEW_FILES:
      return permissions.view_files;
    case MANAGE_WIKI:
      return permissions.manage_wiki;
    case RENAME_WIKI_PAGES:
      return permissions.rename_wiki_pages;
    case DELETE_WIKI_PAGES:
      return permissions.delete_wiki_pages;
    case VIEW_WIKI_PAGES:
      return permissions.view_wiki_pages;
    case EXPORT_WIKI_PAGES:
      return permissions.export_wiki_pages;
    case VIEW_WIKI_EDITS:
      return permissions.view_wiki_edits;
    case EDIT_WIKI_PAGES:
      return permissions.edit_wiki_pages;
    case DELETE_WIKI_PAGES_ATTACHMENTS:
      return permissions.delete_wiki_pages_attachments;
    case PROTECT_WIKI_PAGES:
      return permissions.protect_wiki_pages;
    case MANAGE_REPOSITORY:
      return permissions.manage_repository;
    case BROWSE_REPOSITORY:
      return permissions.browse_repository;
    case VIEW_CHANGESETS:
      return permissions.view_changesets;
    case COMMIT_ACCESS:
      return permissions.commit_access;
    case MANAGE_RELATED_ISSUES:
      return permissions.manage_related_issues;
    case MANAGE_BOARDS:
      return permissions.manage_boards;
    case ADD_MESSAGES:
      return permissions.add_messages;
    case EDIT_MESSAGES:
      return permissions.edit_messages;
    case EDIT_OWN_MESSAGES:
      return permissions.edit_own_messages;
    case DELETE_MESSAGES:
      return permissions.delete_messages;
    case DELETE_OWN_MESSAGES:
      return permissions.delete_own_messages;
    case VIEW_CALENDAR:
      return permissions.view_calendar;
    case VIEW_GANTT:
      return permissions.view_gantt;
  }
}

namespace action {
result user(redmine::args args, redmine::config &config, options options) {
  if (0 == args.count()) {
    fprintf(stderr,
            "usage: redmine issue <action> [args]\n"
            "actions:\n"
            "        list\n"
            "        show <id>\n");
    return FAILURE;
  }

  if (!std::strcmp("list", args[0])) {
    return user_list(++args, config, options);
  }

  if (!std::strcmp("show", args[0])) {
    return user_show(++args, config, options);
  }

  fprintf(stderr, "invalid argument: %s\n", args[0]);
  return INVALID_ARGUMENT;
}

result user_list(redmine::args args, redmine::config &config, options options) {
  CHECK(args.count(), fprintf(stderr, "invalid argument: %s\n", args[0]);
        return FAILURE);

  std::vector<redmine::user> users;
  CHECK_RETURN(query::users(config, options, users));

  printf(
      "  id | name\n"
      "-----|------------------------------------------------------------------"
      "--------\n");
  for (auto &user : users) {
    printf("%4d | %s %s\n", user.id, user.firstname.c_str(),
           user.lastname.c_str());
  }

  return SUCCESS;
}

result user_show(redmine::args args, redmine::config &config, options options) {
  CHECK(0 == args.count(), fprintf(stderr, "missing id\n"));
  CHECK(1 < args.count(), fprintf(stderr, "invalid argument: %s\n", args[1]));

  std::string body;
  CHECK_RETURN(http::get("/users/" + std::string(args[0]) + ".json", config,
                         options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(HAS_OPTION(DEBUG), printf("%s\n", json::write(Root, "  ").c_str()));

  auto User = Root.object().get("user");
  CHECK_JSON_PTR(User, json::TYPE_OBJECT);

  redmine::user user;
  CHECK_RETURN(user.init(User->object()));

  printf("      name: %s %s\n", user.firstname.c_str(), user.lastname.c_str());
  printf("        id: %u\n", user.id);
  printf("     email: %s\n", user.mail.c_str());
  printf("     login: %s\n", user.login.c_str());
  if (!user.api_key.empty()) {
    printf("   api key: %s\n", user.api_key.c_str());
  }
  printf("   created: %s\n", user.created_on.c_str());
  printf("last login: %s\n", user.last_login_on.c_str());
  printf("    status: %u\n", user.status);

  return SUCCESS;
}
}

result query::users(config &config, options options, std::vector<user> &out) {
  std::string body;
  CHECK_RETURN(http::get("/users.json", config, options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);

  CHECK(HAS_OPTION(DEBUG), printf("%s\n", json::write(Root, "  ").c_str()));

  auto Users = Root.object().get("users");
  CHECK_JSON_PTR(Users, json::TYPE_ARRAY);

  for (auto &User : Users->array()) {
    CHECK_JSON_TYPE(User, json::TYPE_OBJECT);

    redmine::user user;
    CHECK_RETURN(user.init(User.object()));

    out.push_back(user);
  }

  return SUCCESS;
}
}
