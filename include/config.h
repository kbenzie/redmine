#ifndef CONFIG_H
#define CONFIG_H

#include <error.h>

#include <string>

struct config_t {
  std::string url;
  std::string key;
};

namespace cmd {
result config(int argc, char **argv);

result config_key(int argc, char **argv);

result config_url(int argc, char **argv);
}

result config_load(config_t *pConfig);

result config_save(config_t &config);

result config_validate(config_t &config);

#endif
