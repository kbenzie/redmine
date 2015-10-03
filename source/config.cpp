#include <config.h>

#include <json/json.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

static std::string config_path() {
  std::string path(std::getenv("HOME"));
#if defined(REDMINE_PLATFORM_LINUX) || defined(REDMINE_PLATFORM_MAC)
  path += "/.redmine.json";
#elif defined(REDMINE_PLATFORM_WINDOWS)
  path += "\\AppData\\Local\\redmine.json";
#endif
  return path;
}

static const char *editor_path() {
  const char *editor = nullptr;
#if defined(REDMINE_PLATFORM_LINUX)
  editor = "vim";
#elif defined(REDMINE_PLATFORM_MAC)
  editor = "open -a TextEdit";
#elif defined(REDMINE_PLATFORM_WINDOWS)
  editor = "notepad";
#endif  // defined(REDMINE_PLATFORM_LINUX) || defined(REDMINE_PLATFORM_MAC)
  ASSERT(!editor, "editor_path() is null!");
  return editor;
}

redmine::config::config()
    : editor(editor_path()),
      profiles(),
      profile_name("default"),
      current(nullptr) {}

redmine::config::profile::profile()
    : name(), url(), key(), port(80), use_ssl(), verify_ssl() {}

redmine::result redmine::config::save() {
  std::ofstream file(config_path());
  CHECK_MSG(!file.is_open(), "could not write config file\n", return FAILURE);
  json::array Profiles;
  for (auto &profile : profiles) {
    json::object Profile;
    Profile.add("name", profile.name);
    Profile.add("url", profile.url);
    Profile.add("key", profile.key);
    Profile.add("port", profile.port);
    Profile.add("use_ssl", profile.use_ssl);
    Profile.add("verify_ssl", profile.verify_ssl);
    Profiles.append(Profile);
  }
  json::object Config;
  Config.add("browser", browser);
  Config.add("editor", editor);
  Config.add("profile_name", profile_name);
  Config.add("profiles", Profiles);
  file << json::write(Config, "  ");
  return SUCCESS;
}

redmine::result redmine::config::load(redmine::options &options) {
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

  // TODO: Actually report config load error to stderr!
  auto Browser = Root.object().get("browser");
  if (Browser) {
    CHECK_JSON_TYPE(*Browser, json::TYPE_STRING);
    browser = Browser->string();
  }

  auto Editor = Root.object().get("editor");
  if (Editor) {
    CHECK_JSON_TYPE(*Editor, json::TYPE_STRING);
    editor = Editor->string();
  }

  auto ProfileName = Root.object().get("profile_name");
  CHECK_JSON_PTR(ProfileName, json::TYPE_STRING);
  profile_name = ProfileName->string();

  auto Profiles = Root.object().get("profiles");
  CHECK_JSON_PTR(Profiles, json::TYPE_ARRAY);

  for (auto &Profile : Profiles->array()) {
    redmine::config::profile profile;

    auto Name = Profile.object().get("name");
    CHECK_JSON_PTR(Name, json::TYPE_STRING);
    profile.name = Name->string();

    auto Url = Profile.object().get("url");
    CHECK_JSON_PTR(Url, json::TYPE_STRING);
    profile.url = Url->string();

    auto Key = Profile.object().get("key");
    CHECK_JSON_PTR(Key, json::TYPE_STRING);
    profile.key = Key->string();

    auto Port = Profile.object().get("port");
    CHECK_JSON_PTR(Port, json::TYPE_NUMBER);
    profile.port = Port->number<uint32_t>();

    auto UseSsl = Profile.object().get("use_ssl");
    if (UseSsl) {
      CHECK_JSON_TYPE(*UseSsl, json::TYPE_BOOL);
      profile.use_ssl = UseSsl->boolean();
    }

    auto VerifySsl = Profile.object().get("verify_ssl");
    if (VerifySsl) {
      CHECK_JSON_TYPE(*VerifySsl, json::TYPE_BOOL);
      profile.verify_ssl = VerifySsl->boolean();
    }

    profiles.push_back(profile);
  }

  for (auto &profile : profiles) {
    if (profile.name == profile_name) {
      current = &profiles.back();
    }
  }

  CHECK(!current,
        fprintf(stderr, "profile_name '%s' does not name a valid profile.\n",
                profile_name.c_str());
        return FAILURE);

  return SUCCESS;
}

redmine::result redmine::action::config(redmine::cl::args &args,
                                        redmine::options &options) {
  if (0 == args.count()) {
    fprintf(stderr,
            "usage: redmine config <action> [args]\n"
            "actions:\n"
            "        new <name>\n"
            "        browser [<path>]\n"
            "        editor [<path>]\n"
            "        profile [<name>]\n"
            "        key [<key>]\n"
            "        url [<url>]\n"
            "        port [<port>]\n"
            "        use-ssl [true|false]\n"
            "        verify-ssl [true|false]\n");
    return FAILURE;
  }

  if (!strcmp("new", args[0])) {
    return config_new(++args, options);
  }

  if (!strcmp("browser", args[0])) {
    return config_browser(++args, options);
  }

  if (!strcmp("editor", args[0])) {
    return config_editor(++args, options);
  }

  if (!strcmp("profile", args[0])) {
    return config_profile(++args, options);
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

redmine::result redmine::action::config_new(redmine::cl::args &args,
                                            redmine::options &options) {
  CHECK(0 == args.count(), fprintf(stderr, "missing config name\n");
        return FAILURE);
  CHECK(1 != args.count(), fprintf(stderr, "invalid argument: %s\n", args[1]);
        return FAILURE);

  config::profile profile;

  printf("url: ");
  std::getline(std::cin, profile.url);

  printf("key: ");
  std::getline(std::cin, profile.key);

  while (!profile.port) {
    printf("port: ");
    std::string str;
    std::getline(std::cin, str);
    char *end = nullptr;
    uint32_t port = std::strtoul(str.c_str(), &end, 10);
    if (str.data() + str.size() == end) {
      profile.port = port;
    }
  }

  printf("use ssl (true|false): ");

  printf("verify ssl (true|false): ");

  return SUCCESS;
}

redmine::result redmine::action::config_browser(redmine::cl::args &args,
                                                redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK(config.load(options), fprintf(stderr, "could not load config file\n");
          return FAILURE);
    printf("using browser %s\n", config.browser.c_str());
    return SUCCESS;
  } else if (1 == args.count()) {
    // NOTE: We don't care if config load fails because we are writing a new
    // editor to it.
    config.load(options);
    config.editor = args[0];
    CHECK(config.save(), fprintf(stderr, "failued to write config file\n");
          return FAILURE);
    printf("using browser %s\n", config.browser.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return INVALID_ARGUMENT;
}

redmine::result redmine::action::config_editor(redmine::cl::args &args,
                                               redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK(config.load(options), fprintf(stderr, "could not load config file\n");
          return FAILURE);
    printf("using editor %s\n", config.editor.c_str());
    return SUCCESS;
  } else if (1 == args.count()) {
    // NOTE: We don't care if config load fails because we are writing a new
    // editor to it.
    config.load(options);
    config.editor = args[0];
    CHECK(config.save(), fprintf(stderr, "failued to write config file\n");
          return FAILURE);
    printf("using editor %s\n", config.editor.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return INVALID_ARGUMENT;
}

redmine::result redmine::action::config_profile(redmine::cl::args &args,
                                                redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK(config.load(options), fprintf(stderr, "could not load config file\n");
          return FAILURE);
    printf("using profile %s\n", config.profile_name.c_str());
    for (auto &profile : config.profiles) {
      if (config.profile_name != profile.name) {
        printf("disabled %s\n", profile.name.c_str());
      }
    }
    return SUCCESS;
  } else if (1 == args.count()) {
    // NOTE: We don't care if config load fails because we are writing a new
    // current profile.
    config.load(options);
    config.profile_name = args[0];
    for (auto &profile : config.profiles) {
      if (config.profile_name == profile.name) {
        config.current = &profile;
      }
    }
    CHECK(config.save(), fprintf(stderr, "failued to write config file\n");
          return FAILURE);
    printf("using profile: %s\n", config.profile_name.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return INVALID_ARGUMENT;
}

redmine::result redmine::action::config_url(redmine::cl::args &args,
                                            redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK(config.load(options), fprintf(stderr, "could not load config file\n");
          return INVALID_CONFIG);
    printf("using url %s\n", config.current->url.c_str());
    return SUCCESS;
  } else if (1 == args.count()) {
    // NOTE: We don't care if config load fails because we are writing a new url
    // to it.
    config.load(options);
    config.current->url = args[0];
    if ('/' == config.current->url.back()) {
      config.current->url.pop_back();
    }
    CHECK(config.save(), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("using url %s\n", config.current->url.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return INVALID_ARGUMENT;
}

redmine::result redmine::action::config_key(redmine::cl::args &args,
                                            redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK_RETURN(config.load(options));
    printf("using api key %s\n", config.current->key.c_str());
    return SUCCESS;
  } else if (1 == args.count()) {
    // NOTE: We don't care if config load fails because we are writing a new key
    // to it.
    config.load(options);
    config.current->key = args[0];
    CHECK(config.save(), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("using api key %s\n", config.current->key.c_str());
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return INVALID_ARGUMENT;
}

redmine::result redmine::action::config_port(redmine::cl::args &args,
                                             redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK_RETURN(config.load(options));
    printf("using port %u\n", config.current->port);
    return SUCCESS;
  } else if (1 == args.count()) {
    // NOTE: We don't care if the config load fails because we are writing a new
    // port to it.
    config.load(options);
    char *end = nullptr;
    config.current->port = strtoul(args[0], &end, 10);
    CHECK(args[0] + strlen(args[0]) != end,
          fprintf(stderr, "invalid argument: %s\n", args[0]);
          return INVALID_ARGUMENT);
    CHECK(config.save(), fprintf(stderr, "failed to write config file\n");
          return FAILURE);
    printf("using port %u\n", config.current->port);
    return SUCCESS;
  }

  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return INVALID_ARGUMENT;
}

redmine::result redmine::action::config_use_ssl(redmine::cl::args &args,
                                                redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK_RETURN(config.load(options));
    printf("ssl is %s\n", (config.current->use_ssl) ? "enabled" : "disabled");
    return SUCCESS;
  } else if (1 == args.count()) {
    config.load(options);
    std::string use_ssl(args[0]);
    if ("true" == use_ssl || "false" == use_ssl) {
      config.current->use_ssl = ('t' == use_ssl[0]) ? true : false;
      config.save();
      printf("ssl is now %s\n",
             (config.current->use_ssl) ? "enabled" : "disabled");
      return SUCCESS;
    }
  }
  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return FAILURE;
}

redmine::result redmine::action::config_verify_ssl(redmine::cl::args &args,
                                                   redmine::options &options) {
  redmine::config config;
  if (0 == args.count()) {
    CHECK_RETURN(config.load(options));
    printf("ssl is verification %s\n",
           (config.current->verify_ssl) ? "enabled" : "disabled");
    return SUCCESS;
  } else {
    config.load(options);
    std::string verify_ssl(args[0]);
    if ("true" == verify_ssl || "false" == verify_ssl) {
      config.current->verify_ssl = ('t' == verify_ssl[0]) ? true : false;
      config.save();
      printf("ssl verification is now %s\n",
             (config.current->use_ssl) ? "enabled" : "disabled");
      return SUCCESS;
    }
  }
  fprintf(stderr, "invalid argument: %s\n", args[1]);
  return FAILURE;
}
