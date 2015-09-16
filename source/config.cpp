#include <config.h>

#include <json/json.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace redmine {
std::string config_path() {
  std::string path(std::getenv("HOME"));
#if defined(REDMINE_PLATFORM_LINUX) || (REDMINE_PLATFORM_MAC)
  path += "/.redmine.json";
#elif defined(REDMINE_PLATFORM_WINDOWS)
  path += "\\AppData\\Local\\redmine.json";
#endif
  return path;
}

config::config() : url(), key(), port(80), use_ssl(), verify_ssl() {}

result config::save() {
  std::ofstream file(config_path());
  CHECK_MSG(!file.is_open(), "could not write config file\n", return FAILURE);
  json::object config;
  config.add({"url", json::value(url)});
  config.add({"key", json::value(key)});
  config.add({"port", json::value(port)});
  config.add({"use_ssl", json::value(use_ssl)});
  config.add({"verify_ssl", json::value(verify_ssl)});
  file << json::write(config, "  ");
  return SUCCESS;
}

result config::load() {
  std::string path(config_path());
  std::ifstream file(path);
  CHECK(!file.is_open(), fprintf(stderr, "could not open: %s\n", path.c_str());
        return INVALID_CONFIG);
  std::string str((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());
  auto Root = json::read(str);
  CHECK(json::TYPE_OBJECT != Root.type(), return INVALID_CONFIG);

  auto Url = Root.object().get("url");
  CHECK_JSON_PTR(Url, json::TYPE_STRING);
  url = Url->string();

  auto Key = Root.object().get("key");
  CHECK_JSON_PTR(Key, json::TYPE_STRING);
  key = Key->string();

  auto Port = Root.object().get("port");
  CHECK_JSON_PTR(Port, json::TYPE_NUMBER);
  port = Port->number<uint32_t>();

  auto UseSsl = Root.object().get("use_ssl");
  if (UseSsl) {
    CHECK_JSON_TYPE((*UseSsl), json::TYPE_BOOL);
    use_ssl = UseSsl->boolean();
  }

  auto VerifySsl = Root.object().get("verify_ssl");
  if (VerifySsl) {
    CHECK_JSON_TYPE((*VerifySsl), json::TYPE_BOOL);
    verify_ssl = VerifySsl->boolean();
  }

  return SUCCESS;
}

namespace action {
result config(int argc, char **argv, options options) {
  if (0 == argc) {
    fprintf(stderr,
            "usage: redmine config <action> [args]\n"
            "actions:\n"
            "        key [<key>]\n"
            "        url [<url>]\n"
            "        port [<port>]\n"
            "        use-ssl [true|false]\n"
            "        verify-ssl [true|false]\n");
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

  if (!strcmp("use-ssl", argv[0])) {
    return config_use_ssl(argc - 1, argv + 1, options);
  }

  if (!strcmp("verify-ssl", argv[0])) {
    return config_verify_ssl(argc - 1, argv + 1, options);
  }

  fprintf(stderr, "invalid argument: %s\n", argv[0]);
  return INVALID_ARGUMENT;
}

result config_url(int argc, char **argv, options options) {
  redmine::config config;
  if (0 == argc) {
    CHECK(config.load(), fprintf(stderr, "could not load config file\n");
          return INVALID_CONFIG);
    printf("using url %s\n", config.url.c_str());
    return SUCCESS;
  } else if (1 == argc) {
    // NOTE: We don't care if config load fails because we are writing a new url
    // to it.
    config.load();
    config.url = argv[0];
    if ('/' == config.url.back()) {
      config.url.pop_back();
    }
    CHECK(config.save(), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("using url %s\n", config.url.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return INVALID_ARGUMENT;
}

result config_key(int argc, char **argv, options options) {
  redmine::config config;
  if (0 == argc) {
    CHECK_RETURN(config.load());
    printf("using api key %s\n", config.key.c_str());
    return SUCCESS;
  } else if (1 == argc) {
    // NOTE: We don't care if config load fails because we are writing a new key
    // to it.
    config.load();
    config.key = argv[0];
    CHECK(config.save(), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("using api key %s\n", config.key.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return INVALID_ARGUMENT;
}

result config_port(int argc, char **argv, options options) {
  redmine::config config;
  if (0 == argc) {
    CHECK_RETURN(config.load());
    printf("using port %u\n", config.port);
    return SUCCESS;
  } else if (1 == argc) {
    // NOTE: We don't care if the config load fails because we are writing a new
    // port to it.
    config.load();
    char *end = nullptr;
    config.port = strtoul(argv[0], &end, 10);
    CHECK(argv[0] + strlen(argv[0]) != end,
          fprintf(stderr, "invalid argument: %s\n", argv[0]);
          return INVALID_ARGUMENT);
    CHECK(config.save(), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("using port %u\n", config.port);
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return INVALID_ARGUMENT;
}

result config_use_ssl(int argc, char **argv, options options) {
  redmine::config config;
  if (0 == argc) {
    CHECK_RETURN(config.load());
    printf("ssl %s\n", (config.use_ssl) ? "enabled" : "disabled");
    return SUCCESS;
  } else if (1 == argc) {
    config.load();
    std::string use_ssl(argv[0]);
    if ("true" == use_ssl || "false" == use_ssl) {
      config.use_ssl = ('t' == use_ssl[0]) ? true : false;
      config.save();
      printf("ssl %s\n", (config.use_ssl) ? "enabled" : "disabled");
      return SUCCESS;
    }
  }
  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return FAILURE;
}

result config_verify_ssl(int argc, char **argv, options options) {
  redmine::config config;
  if (0 == argc) {
    CHECK_RETURN(config.load());
    printf("ssl verification %s\n", (config.verify_ssl) ? "enabled" : "disabled");
    return SUCCESS;
  } else {
    config.load();
    std::string verify_ssl(argv[0]);
    if ("true" == verify_ssl || "false" == verify_ssl) {
      config.verify_ssl = ('t' == verify_ssl[0]) ? true : false;
      config.save();
      printf("ssl verification %s\n",
             (config.use_ssl) ? "enabled" : "disabled");
      return SUCCESS;
    }
  }
  fprintf(stderr, "invalid argument: %s\n", argv[1]);
  return FAILURE;
}
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
