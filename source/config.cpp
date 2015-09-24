#include <config.h>

#include <json/json.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>

namespace redmine {
std::string config_path() {
  std::string path(std::getenv("HOME"));
#if defined(REDMINE_PLATFORM_LINUX) || defined(REDMINE_PLATFORM_MAC)
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
  config.add("url", url);
  config.add("key", key);
  config.add("port", port);
  config.add("use_ssl", use_ssl);
  config.add("verify_ssl", verify_ssl);
  file << json::write(config, "  ");
  return SUCCESS;
}

result config::load(redmine::options &options) {
  std::string path(config_path());
  std::ifstream file(path);
  CHECK(!file.is_open(), fprintf(stderr, "could not open: %s\n", path.c_str());
        return INVALID_CONFIG);
  std::string str((std::istreambuf_iterator<char>(file)),
                  std::istreambuf_iterator<char>());
  auto Root = json::read(str);
  CHECK_JSON_TYPE(Root, json::TYPE_OBJECT);
  CHECK(options.debug, printf("%s\n", json::write(Root, "  ").c_str()));

  // TODO: Properly handle missing config file with interactive creation.

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
result config(redmine::args args, redmine::options &options) {
  if (0 == args.count()) {
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

  if (!strcmp("key", args[0])) {
    return config_key(++args, options);
  }

  if (!strcmp("url", args[0])) {
    return config_url(++args, options);
  }

  if (!strcmp("port", args[0])) {
    return config_port(++args, options);
  }

  if (!strcmp("use-ssl", args[0])) {
    return config_use_ssl(++args, options);
  }

  if (!strcmp("verify-ssl", args[0])) {
    return config_verify_ssl(++args, options);
  }

  fprintf(stderr, "invalid argument: %s\n", args[0]);
  return INVALID_ARGUMENT;
}

result config_url(redmine::args args, redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK(config.load(options), fprintf(stderr, "could not load config file\n");
          return INVALID_CONFIG);
    printf("using url %s\n", config.url.c_str());
    return SUCCESS;
  } else if (1 == args.count()) {
    // NOTE: We don't care if config load fails because we are writing a new url
    // to it.
    config.load(options);
    config.url = args[0];
    if ('/' == config.url.back()) {
      config.url.pop_back();
    }
    CHECK(config.save(), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("using url %s\n", config.url.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return INVALID_ARGUMENT;
}

result config_key(redmine::args args, redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK_RETURN(config.load(options));
    printf("using api key %s\n", config.key.c_str());
    return SUCCESS;
  } else if (1 == args.count()) {
    // NOTE: We don't care if config load fails because we are writing a new key
    // to it.
    config.load(options);
    config.key = args[0];
    CHECK(config.save(), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("using api key %s\n", config.key.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return INVALID_ARGUMENT;
}

result config_port(redmine::args args, redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK_RETURN(config.load(options));
    printf("using port %u\n", config.port);
    return SUCCESS;
  } else if (1 == args.count()) {
    // NOTE: We don't care if the config load fails because we are writing a new
    // port to it.
    config.load(options);
    char *end = nullptr;
    config.port = strtoul(args[0], &end, 10);
    CHECK(args[0] + strlen(args[0]) != end,
          fprintf(stderr, "invalid argument: %s\n", args[0]);
          return INVALID_ARGUMENT);
    CHECK(config.save(), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("using port %u\n", config.port);
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return INVALID_ARGUMENT;
}

result config_use_ssl(redmine::args args, redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK_RETURN(config.load(options));
    printf("ssl is %s\n", (config.use_ssl) ? "enabled" : "disabled");
    return SUCCESS;
  } else if (1 == args.count()) {
    config.load(options);
    std::string use_ssl(args[0]);
    if ("true" == use_ssl || "false" == use_ssl) {
      config.use_ssl = ('t' == use_ssl[0]) ? true : false;
      config.save();
      printf("ssl is now %s\n", (config.use_ssl) ? "enabled" : "disabled");
      return SUCCESS;
    }
  }
  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return FAILURE;
}

result config_verify_ssl(redmine::args args, redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK_RETURN(config.load(options));
    printf("ssl is verification %s\n",
           (config.verify_ssl) ? "enabled" : "disabled");
    return SUCCESS;
  } else {
    config.load(options);
    std::string verify_ssl(args[0]);
    if ("true" == verify_ssl || "false" == verify_ssl) {
      config.verify_ssl = ('t' == verify_ssl[0]) ? true : false;
      config.save();
      printf("ssl verification is now %s\n",
             (config.use_ssl) ? "enabled" : "disabled");
      return SUCCESS;
    }
  }
  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return FAILURE;
}
}  // action
}  // redmine
