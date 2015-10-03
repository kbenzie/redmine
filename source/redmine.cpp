#include <command_line.h>
#include <redmine.h>
#include <config.h>
#include <issue.h>
#include <http.h>
#include <project.h>
#include <user.h>

#include <cstdio>
#include <cstring>

int main(int argc, char **argv) {
  redmine::cl::args args(argc, argv);
  args++;

  redmine::options options;
  int index = 0;
  for (; index < args.count(); ++index) {
    const char *arg = args[index];

    if (!strcmp("--verbose", arg)) {
      options.verbose = true;
      CHECK(args.end() - 1 == &arg, fprintf(stderr, "action required\n");
            return redmine::ACTION_REQUIRED);
      continue;
    }

    if (!strcmp("--debug", arg)) {
      options.debug = true;
      CHECK(args.end() - 1 == &arg, fprintf(stderr, "action required\n");
            return redmine::ACTION_REQUIRED);
      continue;
    }

    if (!strcmp("--debug-http", arg)) {
      options.debug_http = true;
      CHECK(args.end() - 1 == &arg, fprintf(stderr, "action required\n");
            return redmine::ACTION_REQUIRED);
      continue;
    }

    break;
  }
  args += index;

  redmine::http::session http;
  CHECK_RETURN(http.init());

  redmine::config config;
  CHECK_RETURN(config.load(options));

  redmine::current_user user;
  CHECK_RETURN(user.get(config, options));

  const bool use_issue =
      user.can(redmine::ADD_ISSUES) || user.can(redmine::VIEW_ISSUES) ||
      user.can(redmine::EDIT_ISSUES) || user.can(redmine::ADD_ISSUE_NOTES) ||
      user.can(redmine::ADD_ISSUE_WATCHERS);
  const bool use_user = 0 != user.status;

  if (0 == args.count()) {
    printf(
        "usage: redmine [options] <action> [args]\n"
        "actions:\n");
    printf("        config\n");
    printf("        project\n");
    if (use_issue) {
      printf("        issue\n");
    }
    if (use_user) {
      printf("        user\n");
    }
    printf(
        "options:\n"
        "        --verbose - verbose output\n"
        "        --debug - enable debug output\n"
        "        --debug-http - enable http debug output\n");

    return redmine::SUCCESS;
  }

  for (auto arg : args) {
    args++;
    if (!strcmp("config", arg)) {
      return redmine::action::config(args, options);
    }

    if (!strcmp("project", arg)) {
      return redmine::action::project(args, config, options);
    }

    if (use_issue && !strcmp("issue", arg)) {
      return redmine::action::issue(args, config, user, options);
    }

    if (use_user && !strcmp("user", arg)) {
      return redmine::action::user(args, config, options);
    }

    fprintf(stderr, "invalid action: %s\n", arg);
    return redmine::FAILURE;
  }

  fprintf(stderr, "invalid argument: %s\n", args[0]);
  return redmine::INVALID_ARGUMENT;
}

#ifdef REDMINE_DEBUG
const char *redmine::result_string(result result) {
  switch (result) {
    case SUCCESS:
      return "success";
    case FAILURE:
      return "failure";
    case UNSUPPORTED:
      return "unsupported";
    case ACTION_REQUIRED:
      return "action required";
    case INVALID_ARGUMENT:
      return "invalid argument";
    case INVALID_CONFIG:
      return "invalid config";
  }
}
#endif

redmine::result redmine::reference::init(const json::object &object) {
  auto Id = object.get("id");
  CHECK_JSON_PTR(Id, json::TYPE_NUMBER);

  auto Name = object.get("name");
  CHECK_JSON_PTR(Name, json::TYPE_STRING);

  id = Id->number<uint32_t>();
  name = Name->string();

  return SUCCESS;
}
