#ifndef PROJECT_HPP
#define PROJECT_HPP

#include <config.h>
#include <redmine.h>

#include <json/json.hpp>

#include <string>
#include <vector>

namespace redmine {
struct project {
  /// @brief Default constructor.
  project();

  /// @brief Initialise from a json::object.
  ///
  /// @param object Object to initilise redmine::project with.
  ///
  /// @return Returns either redmine::SUCCESS or redmine::FAILURE.
  result init(const json::object &object);

  /// @brief Construct a json::object from this redmine::project.
  ///
  /// @return The constructed json::object.
  json::object jsonify() const;

  /// @brief Equality operator for redmine::project.
  ///
  /// @param other Another redmine::project object.
  ///
  /// @return Returns true if equal, false otherwise.
  bool operator==(const project &other) const;

  /// @brief Equality operator for string.
  ///
  /// @param str String containing either an id, name or identifier.
  ///
  /// @return Returns true if equal, false otherwise.
  bool operator==(const char *str) const;

  uint32_t id;
  std::string name;
  std::string identifier;
  std::string description;
  std::string homepage;
  std::string created_on;
  std::string updated_on;
  reference parent;
};

namespace action {
result project(int argc, char **argv, options options);

result project_list(int argc, char **argv, options options);

result project_new(int argc, char **argv, options options);

result project_show(int argc, char **argv, options options);
}

project *find(std::vector<project> &projects, const char *pattern);

namespace query {
result projects(config &config, options options,
                std::vector<project> &projects);
}
}

#endif
