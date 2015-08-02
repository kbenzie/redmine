#ifndef CONFIG_H
#define CONFIG_H

#include <redmine.h>

#include <string>

struct config_t {
  std::string url;
  std::string key;
};

namespace action {
result_t config(int argc, char **argv, options_t options);

result_t config_key(int argc, char **argv, options_t options);

result_t config_url(int argc, char **argv, options_t options);
}

result_t config_load(config_t &config);

result_t config_save(config_t &config);

result_t config_validate(config_t &config);
#endif
