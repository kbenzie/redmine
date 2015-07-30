#ifndef ISSUE_H
#define ISSUE_H

#include <redmine.h>

namespace cmd {
result_t issue(int argc, char **argv, options_t options);

result_t issue_new(int argc, char **argv, options_t options);

result_t issue_edit(int argc, char **argv, options_t options);

result_t issue_list(int argc, char **argv, options_t options);
}

#endif
