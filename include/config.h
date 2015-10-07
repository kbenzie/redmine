#ifndef CONFIG_H
#define CONFIG_H

#include <command_line.h>
#include <redmine.h>

#include <json/json.hpp>

#include <string>
#include <vector>

namespace redmine {
struct config {
  /// @brief Default constructor.
  config();

  /// @brief Load redmine config file and initilise config object.
  ///
  /// @param options Comand line options.
  ///
  /// @return Either redmine::SUCCESS or redmine::FAILURE.
  result load(redmine::options &options);

  /// @brief Save this config object to the config file.
  ///
  /// @return Either redmine::SUCCESS or redmine::FAILURE.
  result save();

  struct profile {
    profile();

    std::string name;
    std::string url;
    std::string key;
    uint32_t port;
    bool use_ssl;
    bool verify_ssl;
  };

  std::string editor;
  std::string browser;
  std::string profile_name;
  std::vector<config::profile> profiles;
  config::profile *current;
};

/// @brief Interactive setup of redmine::config, writes to config file.
///
/// @return Returns redmine::SUCCESS on succes, redmine::FAILURE otherwise.
result config_interactive(redmine::options &options);

namespace action {
result config(redmine::cl::args &args, redmine::options &options);
result config_new(redmine::cl::args &args, redmine::options &options);
result config_browser(redmine::cl::args &args, redmine::options &options);
result config_editor(redmine::cl::args &args, redmine::options &options);
result config_profile(redmine::cl::args &args, redmine::options &options);
result config_url(redmine::cl::args &args, redmine::options &options);
result config_key(redmine::cl::args &args, redmine::options &options);
result config_port(redmine::cl::args &args, redmine::options &options);
result config_use_ssl(redmine::cl::args &args, redmine::options &options);
result config_verify_ssl(redmine::cl::args &args, redmine::options &options);
}  // action
}  // redmine

#endif
