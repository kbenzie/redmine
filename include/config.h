#ifndef CONFIG_H
#define CONFIG_H

#include <redmine.h>

#include <string>

struct config_t {
  config_t() : url(), key(), port(80), use_ssl(false), verify_ssl(false) {}

  std::string url;
  std::string key;
  uint32_t port;
  bool use_ssl;
  bool verify_ssl;
};

namespace action {
result_t config(int argc, char **argv, options_t options);
result_t config_url(int argc, char **argv, options_t options);
result_t config_key(int argc, char **argv, options_t options);
result_t config_port(int argc, char **argv, options_t options);
result_t config_use_ssl(int argc, char **argv, options_t options);
result_t config_verify_ssl(int argc, char **argv, options_t options);
}

result_t config_load(config_t &config);

result_t config_save(config_t &config);

result_t config_validate(config_t &config);
#endif
