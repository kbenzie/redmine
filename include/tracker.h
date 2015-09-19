#ifndef REDMINE_TRACKER_H
#define REDMINE_TRACKER_H

#include <config.h>

namespace redmine {
namespace query {
result trackers(config &config, options options,
                std::vector<reference> &trackers);
}
}

#endif  // REDMINE_QUERY_H
