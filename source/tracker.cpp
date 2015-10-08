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

#include <http.h>
#include <tracker.h>

namespace redmine {
result query::trackers(redmine::config &config, redmine::options &options,
                       std::vector<reference> &trackers) {
  std::string body;
  CHECK_RETURN(http::get("/trackers.json?offset=0&limit=1000000", config,
                         options, body));

  auto root = json::read(body, false);
  CHECK_JSON_TYPE(root, json::TYPE_OBJECT);

  CHECK(options.debug, printf("%s\n", json::write(root, "  ").c_str()));

  auto Trackers = root.object().get("trackers");
  CHECK_JSON_PTR(Trackers, json::TYPE_ARRAY);

  for (auto &Tracker : Trackers->array()) {
    CHECK_JSON_TYPE(Tracker, json::TYPE_OBJECT);

    reference tracker;
    CHECK_RETURN(tracker.init(Tracker.object()));

    trackers.push_back(tracker);
  }

  return SUCCESS;
}
}
