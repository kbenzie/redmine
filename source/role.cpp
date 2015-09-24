#include <http.h>
#include <membership.h>
#include <role.h>

namespace redmine {
permissions::permissions()
    : id(0),
      name(),
      add_project(false),
      edit_project(false),
      close_project(false),
      select_project_modules(false),
      manage_members(false),
      manage_versions(false),
      add_subprojects(false),
      manage_categories(false),
      view_issues(false),
      add_issues(false),
      edit_issues(false),
      manage_issue_relations(false),
      manage_subtasks(false),
      set_issues_private(false),
      set_own_issues_private(false),
      add_issue_notes(false),
      edit_issue_notes(false),
      edit_own_issue_notes(false),
      view_private_notes(false),
      set_notes_private(false),
      move_issues(false),
      delete_issues(false),
      manage_public_queries(false),
      save_queries(false),
      view_issue_watchers(false),
      add_issue_watchers(false),
      delete_issue_watchers(false),
      log_time(false),
      view_time_entries(false),
      edit_time_entries(false),
      edit_own_time_entries(false),
      manage_project_activities(false),
      manage_news(false),
      comment_news(false),
      add_documents(false),
      edit_documents(false),
      delete_documents(false),
      view_documents(false),
      manage_files(false),
      view_files(false),
      manage_wiki(false),
      rename_wiki_pages(false),
      delete_wiki_pages(false),
      view_wiki_pages(false),
      export_wiki_pages(false),
      view_wiki_edits(false),
      edit_wiki_pages(false),
      delete_wiki_pages_attachments(false),
      protect_wiki_pages(false),
      manage_repository(false),
      browse_repository(false),
      view_changesets(false),
      commit_access(false),
      manage_related_issues(false),
      manage_boards(false),
      add_messages(false),
      edit_messages(false),
      edit_own_messages(false),
      delete_messages(false),
      delete_own_messages(false),
      view_calendar(false),
      view_gantt(false) {}

result permissions::get(const uint32_t role, const redmine::config &config,
                        redmine::options &options) {
  std::string body;
  CHECK_RETURN(http::get("/roles/" + std::to_string(role) + ".json", config,
                         options, body));
  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(options.debug, printf("%s\n", json::write(Root, "  ").c_str()));

  auto Role = Root.object().get("role");
  CHECK_JSON_PTR(Role, json::TYPE_OBJECT);

  auto Id = Role->object().get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);
  id = Id->number<uint32_t>();

  auto Name = Role->object().get("name");
  CHECK_JSON_PTR(Name, json::TYPE_STRING);
  name = Name->string();

  auto Permissions = Role->object().get("permissions");
  CHECK_JSON_PTR(Permissions, json::TYPE_ARRAY);

  for (auto &Permission : Permissions->array()) {
    CHECK_JSON_TYPE(Permission, json::TYPE_STRING);

#define PERMISSION(NAME)              \
  if (Permission.string() == #NAME) { \
    NAME = true;                      \
  }

    // Project
    PERMISSION(add_project);
    PERMISSION(edit_project);
    PERMISSION(close_project);
    PERMISSION(select_project_modules);
    PERMISSION(manage_members);
    PERMISSION(manage_versions);
    PERMISSION(add_subprojects);

    // Issue
    PERMISSION(manage_categories);
    PERMISSION(view_issues);
    PERMISSION(add_issues);
    PERMISSION(edit_issues);
    PERMISSION(manage_issue_relations);
    PERMISSION(manage_subtasks);
    PERMISSION(set_issues_private);
    PERMISSION(set_own_issues_private);
    PERMISSION(add_issue_notes);
    PERMISSION(edit_issue_notes);
    PERMISSION(edit_own_issue_notes);
    PERMISSION(view_private_notes);
    PERMISSION(set_notes_private);
    PERMISSION(move_issues);
    PERMISSION(delete_issues);
    PERMISSION(manage_public_queries);
    PERMISSION(save_queries);
    PERMISSION(view_issue_watchers);
    PERMISSION(add_issue_watchers);
    PERMISSION(delete_issue_watchers);

    // Time Tracking
    PERMISSION(log_time);
    PERMISSION(view_time_entries);
    PERMISSION(edit_time_entries);
    PERMISSION(edit_own_time_entries);
    PERMISSION(manage_project_activities);

    // News
    PERMISSION(manage_news);
    PERMISSION(comment_news);

    // Document
    PERMISSION(add_documents);
    PERMISSION(edit_documents);
    PERMISSION(delete_documents);
    PERMISSION(view_documents);

    // File
    PERMISSION(manage_files);
    PERMISSION(view_files);

    // Wiki
    PERMISSION(manage_wiki);
    PERMISSION(rename_wiki_pages);
    PERMISSION(delete_wiki_pages);
    PERMISSION(view_wiki_pages);
    PERMISSION(export_wiki_pages);
    PERMISSION(view_wiki_edits);
    PERMISSION(edit_wiki_pages);
    PERMISSION(delete_wiki_pages_attachments);
    PERMISSION(protect_wiki_pages);

    // Repository
    PERMISSION(manage_repository);
    PERMISSION(browse_repository);
    PERMISSION(view_changesets);
    PERMISSION(commit_access);
    PERMISSION(manage_related_issues);

    // Forum
    PERMISSION(manage_boards);
    PERMISSION(add_messages);
    PERMISSION(edit_messages);
    PERMISSION(edit_own_messages);
    PERMISSION(delete_messages);
    PERMISSION(delete_own_messages);

    // Calendar
    PERMISSION(view_calendar);

    // Gantt
    PERMISSION(view_gantt);
#undef PERMISSION
  }

  return SUCCESS;
}

redmine::permissions &redmine::permissions::operator|=(
    const permissions &other) {
#define OR_ASSIGN(PERMISSION)      \
  if (other.PERMISSION) {          \
    PERMISSION = other.PERMISSION; \
  }
  OR_ASSIGN(add_project)
  OR_ASSIGN(edit_project)
  OR_ASSIGN(close_project)
  OR_ASSIGN(select_project_modules)
  OR_ASSIGN(manage_members)
  OR_ASSIGN(manage_versions)
  OR_ASSIGN(add_subprojects)
  OR_ASSIGN(manage_categories)
  OR_ASSIGN(view_issues)
  OR_ASSIGN(add_issues)
  OR_ASSIGN(edit_issues)
  OR_ASSIGN(manage_issue_relations)
  OR_ASSIGN(manage_subtasks)
  OR_ASSIGN(set_issues_private)
  OR_ASSIGN(set_own_issues_private)
  OR_ASSIGN(add_issue_notes)
  OR_ASSIGN(edit_issue_notes)
  OR_ASSIGN(edit_own_issue_notes)
  OR_ASSIGN(view_private_notes)
  OR_ASSIGN(set_notes_private)
  OR_ASSIGN(move_issues)
  OR_ASSIGN(delete_issues)
  OR_ASSIGN(manage_public_queries)
  OR_ASSIGN(save_queries)
  OR_ASSIGN(view_issue_watchers)
  OR_ASSIGN(add_issue_watchers)
  OR_ASSIGN(delete_issue_watchers)
  OR_ASSIGN(log_time)
  OR_ASSIGN(view_time_entries)
  OR_ASSIGN(edit_time_entries)
  OR_ASSIGN(edit_own_time_entries)
  OR_ASSIGN(manage_project_activities)
  OR_ASSIGN(manage_news)
  OR_ASSIGN(comment_news)
  OR_ASSIGN(add_documents)
  OR_ASSIGN(edit_documents)
  OR_ASSIGN(delete_documents)
  OR_ASSIGN(view_documents)
  OR_ASSIGN(manage_files)
  OR_ASSIGN(view_files)
  OR_ASSIGN(manage_wiki)
  OR_ASSIGN(rename_wiki_pages)
  OR_ASSIGN(delete_wiki_pages)
  OR_ASSIGN(view_wiki_pages)
  OR_ASSIGN(export_wiki_pages)
  OR_ASSIGN(view_wiki_edits)
  OR_ASSIGN(edit_wiki_pages)
  OR_ASSIGN(delete_wiki_pages_attachments)
  OR_ASSIGN(protect_wiki_pages)
  OR_ASSIGN(manage_repository)
  OR_ASSIGN(browse_repository)
  OR_ASSIGN(view_changesets)
  OR_ASSIGN(commit_access)
  OR_ASSIGN(manage_related_issues)
  OR_ASSIGN(manage_boards)
  OR_ASSIGN(add_messages)
  OR_ASSIGN(edit_messages)
  OR_ASSIGN(edit_own_messages)
  OR_ASSIGN(delete_messages)
  OR_ASSIGN(delete_own_messages)
  OR_ASSIGN(view_calendar)
  OR_ASSIGN(view_gantt)
#undef OR_ASSIGN
  return *this;
}

namespace query {
result roles(const redmine::config &config, redmine::options options,
             std::vector<redmine::reference> &roles) {
  std::string body;
  CHECK_RETURN(http::get("/roles.json", config, options, body));

  auto Root = json::read(body, false);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(options.debug, printf("%s\n", json::write(Root, "  ").c_str()));

  auto Roles = Root.object().get("roles");
  CHECK_JSON_PTR(Roles, json::TYPE_ARRAY);

  for (auto &Role : Roles->array()) {
    CHECK_JSON_TYPE(Role, json::TYPE_OBJECT);

    redmine::reference role;
    CHECK_RETURN(role.init(Role.object()));

    roles.push_back(role);
  }

  return SUCCESS;
}
}  // query
}  // redmine
