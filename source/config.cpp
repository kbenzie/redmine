#include <config.h>

#include <json/json.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace redmine {
namespace action {
result config(int argc, char **argv, options options) {
  if (0 == argc) {
    fprintf(stderr,
            "usage: redmine config <action> [args]\n"
            "actions:\n"
            "        key [new key]\n"
            "        url [new url]\n"
            "        port <port>"
            "        use_ssl <true|false>"
            "        verify_ssl <true|false>");
    return FAILURE;
  }

  if (!strcmp("key", argv[0])) {
    return config_key(argc - 1, argv + 1, options);
  }

  if (!strcmp("url", argv[0])) {
    return config_url(argc - 1, argv + 1, options);
  }

  if (!strcmp("port", argv[0])) {
    return config_port(argc - 1, argv + 1, options);
  }

  if (!strcmp("use_ssl", argv[0])) {
    return config_use_ssl(argc - 1, argv + 1, options);
  }

  if (!strcmp("verify_ssl", argv[0])) {
    return config_verify_ssl(argc - 1, argv + 1, options);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result config_url(int argc, char **argv, options options) {
  redmine::config config;
  if (0 == argc) {
    CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
          return INVALID_CONFIG);
    printf("url: %s\n", config.url.c_str());
    CHECK(config_validate(config), return INVALID_CONFIG);
    return SUCCESS;
  } else if (1 == argc) {
    // NOTE: We don't care if config load fails because we are writing a new url
    // to it.
    config_load(config);
    config.url = argv[0];
    if ('/' == config.url.back()) {
      config.url.pop_back();
    }
    CHECK(config_save(config), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("set url: %s\n", config.url.c_str());
    CHECK(config_validate(config), return INVALID_CONFIG);
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return INVALID_ARGUMENT;
}

result config_key(int argc, char **argv, options options) {
  redmine::config config;
  if (0 == argc) {
    CHECK(config_load(config), fprintf(stderr, "invalid config file\n");
          return INVALID_CONFIG);
    printf("key: %s\n", config.key.c_str());
    return SUCCESS;
  } else if (1 == argc) {
    // NOTE: We don't care if config load fails because we are writing a new key
    // to it.
    config_load(config);
    config.key = argv[0];
    CHECK(config_save(config), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("set key: %s\n", config.key.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return INVALID_ARGUMENT;
}

result config_port(int argc, char **argv, options options) {
  redmine::config config;
  if (0 == argc) {
    CHECK(config_load(config), fprintf(stderr, "invalid config file\n"));
    printf("port: %u\n", config.port);
    return SUCCESS;
  } else if (1 == argc) {
    // NOTE: We don't care if the config load fails because we are writign a new
    // port to it.
    config_load(config);
    char *end = nullptr;
    config.port = strtoul(argv[0], &end, 10);
    CHECK(argv[0] + strlen(argv[0]) != end,
          fprintf(stderr, "invalid argument: %s\n", argv[0]);
          return INVALID_ARGUMENT);
    CHECK(config_save(config), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("set port: %u\n", config.port);
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return INVALID_ARGUMENT;
}

result config_use_ssl(int argc, char **argv, options options) {
  return UNSUPPORTED;
}

result config_verify_ssl(int argc, char **argv, options options) {
  return UNSUPPORTED;
}
}

std::string config_path() {
  std::string path(std::getenv("HOME"));
#if defined(REDMINE_PLATFORM_LINUX) || (REDMINE_PLATFORM_MAC)
  path += "/.redmine.json";
#elif defined(REDMINE_PLATFORM_WINDOWS)
  path += "\\AppData\\Local\\redmine.json";
#endif
  return path;
}

result config_load(config &out) {
  std::string path(config_path());
  std::ifstream file(path);
  CHECK(!file.is_open(), fprintf(stderr, "could not open: %s\n", path.c_str());
        return INVALID_CONFIG);
  std::string str((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());
  json::value value = json::read(str);
  CHECK(json::TYPE_OBJECT != value.type(), return INVALID_CONFIG);
  json::object &config = value.object();
  if (auto url = config.get("url")) {
    CHECK(json::TYPE_STRING != url->type(), return INVALID_CONFIG);
    out.url = url->string();
  }
  if (auto key = config.get("key")) {
    CHECK(json::TYPE_STRING != key->type(), return INVALID_CONFIG);
    out.key = key->string();
  }
  if (auto port = config.get("port")) {
    CHECK(json::TYPE_NUMBER != port->type(), return INVALID_CONFIG);
    out.port = port->number<uint32_t>();
  }
  if (auto use_ssl = config.get("use_ssl")) {
    CHECK(json::TYPE_BOOL != use_ssl->type(), return INVALID_CONFIG);
    out.use_ssl = use_ssl->boolean();
  }
  if (auto verify_ssl = config.get("verify_ssl")) {
    CHECK(json::TYPE_BOOL != verify_ssl->type(), return INVALID_CONFIG);
    out.verify_ssl = verify_ssl->boolean();
  }
  return SUCCESS;
}

result config_save(config &config) {
  std::string path(config_path());
  std::ofstream file(path);
  CHECK(!file.is_open(), fprintf(stderr, "could not open: %s\n", path.c_str());
        return FAILURE);
  json::value json{
      json::object{json::pair("url", json::value(config.url)),
                   json::pair("key", json::value(config.key)),
                   json::pair("port", json::value(config.port)),
                   json::pair("use_ssl", json::value(config.use_ssl)),
                   json::pair("verify_ssl", json::value(config.verify_ssl))}};
  file << json::write(json, "  ");
  return SUCCESS;
}

result config_validate(config &config) {
  redmine::result error = SUCCESS;
  if (!config.key.size()) {
    fprintf(stderr, "key is empty, set using: redmine config key <key>\n");
    error = INVALID_CONFIG;
  }
  if (!config.key.size()) {
    fprintf(stderr, "url is empty, set using: redmine config url <url>\n");
    error = INVALID_CONFIG;
  }
  // TODO: Validate port
  return error;
}
}
