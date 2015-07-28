#ifndef ISSUE_H
#define ISSUE_H

#include <error.h>

namespace cmd {
result issue(int argc, char **argv);

result issue_new(int argc, char **argv);

result issue_edit(int argc, char **argv);

result issue_list(int argc, char **argv);
}

#endif
