#ifndef REDMINE_ROLE_H
#define REDMINE_ROLE_H

#include <config.h>

namespace redmine {
enum permisson {
  USE_PROJECT,
  USE_ISSUE,
  USE_TIME_TRACKING,
  USE_NEWS,
  USE_DOCUMENT,
  USE_FILE,
  USE_WIKI,
  USE_REPOSITORY,
  USE_FORUM,
  USE_CALENDAR,
  USE_GANTT,
  ADD_PROJECT,
  EDIT_PROJECT,
  CLOSE_PROJECT,
  SELECT_PROJECT_MODULES,
  MANAGE_MEMBERS,
  MANAGE_VERSIONS,
  ADD_SUBPROJECTS,
  MANAGE_CATEGORIES,
  VIEW_ISSUES,
  ADD_ISSUES,
  EDIT_ISSUES,
  MANAGE_ISSUE_RELATIONS,
  MANAGE_SUBTASKS,
  SET_ISSUES_PRIVATE,
  SET_OWN_ISSUES_PRIVATE,
  ADD_ISSUE_NOTES,
  EDIT_ISSUE_NOTES,
  EDIT_OWN_ISSUE_NOTES,
  VIEW_PRIVATE_NOTES,
  SET_NOTES_PRIVATE,
  MOVE_ISSUES,
  DELETE_ISSUES,
  MANAGE_PUBLIC_QUERIES,
  SAVE_QUERIES,
  VIEW_ISSUE_WATCHERS,
  ADD_ISSUE_WATCHERS,
  DELETE_ISSUE_WATCHERS,
  LOG_TIME,
  VIEW_TIME_ENTRIES,
  EDIT_TIME_ENTRIES,
  EDIT_OWN_TIME_ENTRIES,
  MANAGE_PROJECT_ACTIVITIES,
  MANAGE_NEWS,
  COMMENT_NEWS,
  ADD_DOCUMENTS,
  EDIT_DOCUMENTS,
  DELETE_DOCUMENTS,
  VIEW_DOCUMENTS,
  MANAGE_FILES,
  VIEW_FILES,
  MANAGE_WIKI,
  RENAME_WIKI_PAGES,
  DELETE_WIKI_PAGES,
  VIEW_WIKI_PAGES,
  EXPORT_WIKI_PAGES,
  VIEW_WIKI_EDITS,
  EDIT_WIKI_PAGES,
  DELETE_WIKI_PAGES_ATTACHMENTS,
  PROTECT_WIKI_PAGES,
  MANAGE_REPOSITORY,
  BROWSE_REPOSITORY,
  VIEW_CHANGESETS,
  COMMIT_ACCESS,
  MANAGE_RELATED_ISSUES,
  MANAGE_BOARDS,
  ADD_MESSAGES,
  EDIT_MESSAGES,
  EDIT_OWN_MESSAGES,
  DELETE_MESSAGES,
  DELETE_OWN_MESSAGES,
  VIEW_CALENDAR,
  VIEW_GANTT,
};

struct permissions {
  permissions();

  result get(const uint32_t role, const redmine::config &config,
             redmine::options &options);

  permissions &operator|=(const permissions &other);

  uint32_t id;
  std::string name;

  bool use_project;
  bool use_issue;
  bool use_time_tracking;
  bool use_news;
  bool use_document;
  bool use_file;
  bool use_wiki;
  bool use_repository;
  bool use_forum;
  bool use_calendar;
  bool use_gantt;

  // Project
  bool add_project;
  bool edit_project;
  bool close_project;
  bool select_project_modules;
  bool manage_members;
  bool manage_versions;
  bool add_subprojects;

  // Issue
  bool manage_categories;
  bool view_issues;
  bool add_issues;
  bool edit_issues;
  bool manage_issue_relations;
  bool manage_subtasks;
  bool set_issues_private;
  bool set_own_issues_private;
  bool add_issue_notes;
  bool edit_issue_notes;
  bool edit_own_issue_notes;
  bool view_private_notes;
  bool set_notes_private;
  bool move_issues;
  bool delete_issues;
  bool manage_public_queries;
  bool save_queries;
  bool view_issue_watchers;
  bool add_issue_watchers;
  bool delete_issue_watchers;

  // Time Tracking
  bool log_time;
  bool view_time_entries;
  bool edit_time_entries;
  bool edit_own_time_entries;
  bool manage_project_activities;

  // News
  bool manage_news;
  bool comment_news;

  // Document
  bool add_documents;
  bool edit_documents;
  bool delete_documents;
  bool view_documents;

  // File
  bool manage_files;
  bool view_files;

  // Wiki
  bool manage_wiki;
  bool rename_wiki_pages;
  bool delete_wiki_pages;
  bool view_wiki_pages;
  bool export_wiki_pages;
  bool view_wiki_edits;
  bool edit_wiki_pages;
  bool delete_wiki_pages_attachments;
  bool protect_wiki_pages;

  // Repository
  bool manage_repository;
  bool browse_repository;
  bool view_changesets;
  bool commit_access;
  bool manage_related_issues;

  // Forum
  bool manage_boards;
  bool add_messages;
  bool edit_messages;
  bool edit_own_messages;
  bool delete_messages;
  bool delete_own_messages;

  // Calendar
  bool view_calendar;

  // Gantt
  bool view_gantt;
};

namespace query {
result roles(const redmine::config &config, redmine::options &options,
             std::vector<redmine::reference> &roles);
}
}

#endif
