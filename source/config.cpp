#include <config.h>

#include <json/json.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

result cmd::config(int argc, char **argv) {
  if (0 == argc) {
    fprintf(stderr, "usage: redmine config <action> [args]\n"
                    "actions:\n"
                    "        key [new key]\n"
                    "        url [new url]\n");
    return FAILURE;
  }

  if (!strcmp("key", argv[0])) {
    return config_key(argc - 1, argv + 1);
  }

  if (!strcmp("url", argv[0])) {
    return config_url(argc - 1, argv + 1);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result cmd::config_key(int argc, char **argv) {
  if (0 == argc) {
    config_t config;
    CHECK(config_load(&config), fprintf(stderr, "invalid config file\n");
          return INVALID_CONFIG);
    printf("key: %s\n", config.key.c_str());
    return SUCCESS;
  } else if (1 == argc) {
    config_t config;
    // NOTE: We don't care if config load fails because we are writing a new key
    // to it.
    config_load(&config);
    config.key = argv[0];
    CHECK(config_save(config), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("set key: %s\n", config.key.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return INVALID_ARGUMENT;
}

result cmd::config_url(int argc, char **argv) {
  if (0 == argc) {
    config_t config;
    CHECK(config_load(&config), fprintf(stderr, "invalid config file\n");
          return INVALID_CONFIG);
    printf("url: %s\n", config.url.c_str());
    return SUCCESS;
  } else if (1 == argc) {
    config_t config;
    // NOTE: We don't care if config load fails because we are writing a new url
    // to it.
    config_load(&config);
    config.url = argv[0];
    CHECK(config_save(config), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("set url: %s\n", config.url.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return INVALID_ARGUMENT;
}

std::string config_path() {
  std::string path(std::getenv("HOME"));
  return path + "/.redmine.json";
}

result config_load(config_t *pConfig) {
  CHECK(!pConfig, return INVALID_POINTER);
  std::ifstream file(config_path());
  std::string str((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());
  json::value value = json::read(str);
  CHECK(json::TYPE_OBJECT != value.type(), return INVALID_CONFIG);
  json::object &config = value.object();
  json::value *url = config.get("url");
  CHECK(json::TYPE_STRING != url->type(), return INVALID_CONFIG);
  json::value *key = config.get("key");
  CHECK(json::TYPE_STRING != key->type(), return INVALID_CONFIG);
  pConfig->url = url->string();
  pConfig->key = key->string();
  return SUCCESS;
}

result config_save(config_t &config) {
  std::string path(config_path());
  std::ofstream file(path);
  if (!file.is_open()) {
    fprintf(stderr, "could not open: %s\n", path.c_str());
    return FAILURE;
  }
  json::value json{json::object{json::pair("url", json::value(config.url)),
                                json::pair("key", json::value(config.key))}};
  file << json::write(json, "  ");
  return SUCCESS;
}

result config_validate(config_t &config) {
  result error = SUCCESS;
  if (!config.key.size()) {
    fprintf(stderr, "key is empty, set using: redmine config key <key>\n");
    error = INVALID_CONFIG;
  }
  if (!config.key.size()) {
    fprintf(stderr, "url is empty, set using: redmine config url <url>\n");
    error = INVALID_CONFIG;
  }
  return error;
}
