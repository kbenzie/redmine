#ifndef PROJECT_H
#define PROJECT_H

#include <error.h>

namespace cmd {
result project(int argc, char **argv);

result project_show(int argc, char **argv);

result project_list(int argc, char **argv);
}

#endif
