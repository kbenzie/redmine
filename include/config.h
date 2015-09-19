#ifndef CONFIG_H
#define CONFIG_H

#include <args.h>
#include <redmine.h>

#include <json/json.hpp>

#include <string>

namespace redmine {
struct config {
  /// @brief Default constructor.
  config();

  /// @brief Load redmine config file and initilise config object.
  ///
  /// @param options Comand line options.
  ///
  /// @return Either SUCCESS or FAILURE.
  result load(redmine::options options);

  /// @brief Save this config object to the config file.
  ///
  /// @return Either SUCCESS or FAILURE.
  result save();

  std::string url;
  std::string key;
  // TODO: std::string editor;
  // TODO: std::string browser;
  uint32_t port;
  bool use_ssl;
  bool verify_ssl;
};

namespace action {
result config(redmine::args args, options options);
result config_url(redmine::args args, options options);
result config_key(redmine::args args, options options);
result config_port(redmine::args args, options options);
result config_use_ssl(redmine::args args, options options);
result config_verify_ssl(redmine::args args, options options);
}
}

#endif
