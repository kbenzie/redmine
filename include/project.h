#ifndef PROJECT_H
#define PROJECT_H

#include <redmine.h>

namespace action {
result_t project(int argc, char **argv, options_t options);

result_t project_show(int argc, char **argv, options_t options);

result_t project_list(int argc, char **argv, options_t options);
}

#endif
