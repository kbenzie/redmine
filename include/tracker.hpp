#ifndef REDMINE_TRACKER_HPP
#define REDMINE_TRACKER_HPP

#include <config.h>

namespace redmine {
namespace query {
result trackers(config &config, options options,
                std::vector<reference> &trackers);
}
}

#endif  // REDMINE_QUERY_H
