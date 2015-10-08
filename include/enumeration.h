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

#ifndef REDMINE_ENUMERATIONS_H
#define REDMINE_ENUMERATIONS_H

#include <redmine.h>
#include <config.h>

namespace redmine {
struct enumeration {
  uint32_t id;
  std::string name;
  bool is_default;
};

namespace query {
/// @brief Query the list of redmine issue priorities.
///
/// @param config User configuration.
/// @param options Command line options.
/// @param issue_statuses Returned list of issue statuses.
///
/// @return Returns SUCCESS or FAILURE.
result issue_priorities(redmine::config &config, redmine::options &options,
                        std::vector<enumeration> &issue_statuses);

/// @brief Query the list of redmine time entry activities.
///
/// @param config User configuration.
/// @param options Command line options.
/// @param time_entry_activities Returned list of time entry activities.
///
/// @return Returns SUCCESS or FAILURE.
result time_entry_activities(redmine::config &config, redmine::options &options,
                             std::vector<enumeration> &time_entry_activities);
}
}

#endif
