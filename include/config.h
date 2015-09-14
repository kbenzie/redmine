#ifndef CONFIG_H
#define CONFIG_H

#include <redmine.h>

#include <string>

namespace redmine {
struct config {
  config() : url(), key(), port(80), use_ssl(false), verify_ssl(false) {}

  std::string url;
  std::string key;
  uint32_t port;
  bool use_ssl;
  bool verify_ssl;
};

namespace action {
result config(int argc, char **argv, options options);
result config_url(int argc, char **argv, options options);
result config_key(int argc, char **argv, options options);
result config_port(int argc, char **argv, options options);
result config_use_ssl(int argc, char **argv, options options);
result config_verify_ssl(int argc, char **argv, options options);
}

result config_load(config &config);

result config_save(config &config);

result config_validate(config &config);
}

#endif
