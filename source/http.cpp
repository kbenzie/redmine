#include <http.h>
#include <redmine.h>

#include <curl/curl.h>

namespace redmine {
result print_curl_error(CURLcode error, const char *file, const int line);
#define CURL_CHECK_RETURN(EXPRESSION)                                    \
  if (result error = print_curl_error(EXPRESSION, __FILE__, __LINE__)) { \
    return error;                                                        \
  }
result print_http_error(const http::status error);

redmine::result redmine::http::session::init() {
  CURL_CHECK_RETURN(curl_global_init(CURL_GLOBAL_ALL));
  return SUCCESS;
}

http::session::~session() { curl_global_cleanup(); }

struct curl_raii {
  curl_raii() : handle(curl_easy_init()) {}

  ~curl_raii() {
    if (handle) {
      curl_easy_cleanup(handle);
    }
  }

  bool valid() { return handle; }

  operator CURL *() { return handle; }

  CURL *handle;
};

struct read_state {
  read_state(const std::string &str) : str(str), index(0) {}

  const std::string &str;
  size_t index;
};

size_t read(char *ptr, size_t size, size_t count, void *data) {
  read_state *state = static_cast<read_state *>(data);
  curl_off_t read = size * count;
  if (read > (state->str.size() - state->index)) {
    read = state->str.size() - state->index;
  }
  if (read) {
    std::memcpy(ptr, &state->str[state->index], read);
    state->index += read;
  }
  return read;
}

size_t write(void *ptr, size_t size, size_t count, void *data) {
  std::string *str = static_cast<std::string *>(data);
  char *in = static_cast<char *>(ptr);
  const size_t bytes = size * count;
  str->append(in, in + bytes);
  return bytes;
}

struct curl_slist *create_header(const config &config, size_t length = 0) {
  struct curl_slist *header = nullptr;
  std::string api_key_header("X-Redmine-API-Key: ");
  api_key_header += config.key;
  header = curl_slist_append(header, api_key_header.c_str());
  header = curl_slist_append(header, "Content-Type: application/json");
  if (length) {
    std::string content_length("Content-Length: ");
    content_length += std::to_string(length);
    header = curl_slist_append(header, content_length.c_str());
  }
  return header;
}

result set_options(CURL *curl, const std::string &path,
                   const redmine::config &config, redmine::options &options) {
  std::string url = config.url + path;
  CHECK(options.debug, printf("%s\n", url.c_str()));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_URL, url.c_str()));
  auto header = create_header(config);
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header));
  if (config.use_ssl) {
    CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL));
    CURL_CHECK_RETURN(
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, config.verify_ssl));
  }
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_PORT, config.port));
  if (options.debug_http) {
    CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_VERBOSE, true));
  }
  return SUCCESS;
}

result http::get(const std::string &path, const config &config,
                 redmine::options &options, std::string &body) {
  CHECK(options.debug, printf("%s\n", path.c_str()));
  curl_raii curl;
  CHECK(!curl.valid(), fprintf(stderr, "curl init failed\n"); return FAILURE);
  CHECK_RETURN(set_options(curl, path, config, options));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_HTTPGET, 1));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body));

  CURL_CHECK_RETURN(curl_easy_perform(curl));
  http::status status;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
  CHECK(http::code::OK != status, print_http_error(status); return FAILURE);

  CHECK(options.debug, printf("body: %s\n", body.c_str()));

  return SUCCESS;
}

result http::post(const std::string &path, const config &config,
                  redmine::options &options, const http::status expected,
                  const std::string &str, std::string &body) {
  CHECK(options.debug, printf("%s\n", path.c_str()));
  curl_raii curl;
  CHECK(!curl.valid(), fprintf(stderr, "curl init failed\n"); return FAILURE);
  CHECK_RETURN(set_options(curl, path, config, options));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str.c_str()));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body));

  CURL_CHECK_RETURN(curl_easy_perform(curl));
  status status;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

  CHECK(options.debug, printf("body: %s\n", body.c_str()));
  CHECK(expected != status, print_http_error(status); return FAILURE);

  return SUCCESS;
}

result http::put(const std::string &path, const redmine::config &config,
                 redmine::options &options, const http::status expected,
                 const std::string &data) {
  curl_raii curl;
  CHECK(!curl.valid(), fprintf(stderr, "curl init failed\n"); return FAILURE);
  CHECK_RETURN(set_options(curl, path, config, options));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_PUT, true));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_READFUNCTION, read));
  read_state state(data);
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_READDATA, &state));

  CURL_CHECK_RETURN(curl_easy_perform(curl));
  http::status status;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

  CHECK(expected != status, print_http_error(status); return FAILURE);

  return SUCCESS;
}

result print_curl_error(CURLcode error, const char *file, const int line) {
#define CASE(ERROR)                                      \
  case ERROR:                                            \
    fprintf(stderr, "%s: %d: %s\n", file, line, #ERROR); \
    return FAILURE;
  switch (error) {
    CASE(CURLE_UNSUPPORTED_PROTOCOL)
    CASE(CURLE_FAILED_INIT)
    CASE(CURLE_URL_MALFORMAT)
    CASE(CURLE_NOT_BUILT_IN)
    CASE(CURLE_COULDNT_RESOLVE_PROXY)
    CASE(CURLE_COULDNT_RESOLVE_HOST)
    CASE(CURLE_COULDNT_CONNECT)
    CASE(CURLE_FTP_WEIRD_SERVER_REPLY)
    CASE(CURLE_REMOTE_ACCESS_DENIED)
    CASE(CURLE_FTP_ACCEPT_FAILED)
    CASE(CURLE_FTP_WEIRD_PASS_REPLY)
    CASE(CURLE_FTP_ACCEPT_TIMEOUT)
    CASE(CURLE_FTP_WEIRD_PASV_REPLY)
    CASE(CURLE_FTP_WEIRD_227_FORMAT)
    CASE(CURLE_FTP_CANT_GET_HOST)
    CASE(CURLE_HTTP2)
    CASE(CURLE_FTP_COULDNT_SET_TYPE)
    CASE(CURLE_PARTIAL_FILE)
    CASE(CURLE_FTP_COULDNT_RETR_FILE)
    CASE(CURLE_OBSOLETE20)
    CASE(CURLE_QUOTE_ERROR)
    CASE(CURLE_HTTP_RETURNED_ERROR)
    CASE(CURLE_WRITE_ERROR)
    CASE(CURLE_OBSOLETE24)
    CASE(CURLE_UPLOAD_FAILED)
    CASE(CURLE_READ_ERROR)
    CASE(CURLE_OUT_OF_MEMORY)
    CASE(CURLE_OPERATION_TIMEDOUT)
    CASE(CURLE_OBSOLETE29)
    CASE(CURLE_FTP_PORT_FAILED)
    CASE(CURLE_FTP_COULDNT_USE_REST)
    CASE(CURLE_OBSOLETE32)
    CASE(CURLE_RANGE_ERROR)
    CASE(CURLE_HTTP_POST_ERROR)
    CASE(CURLE_SSL_CONNECT_ERROR)
    CASE(CURLE_BAD_DOWNLOAD_RESUME)
    CASE(CURLE_FILE_COULDNT_READ_FILE)
    CASE(CURLE_LDAP_CANNOT_BIND)
    CASE(CURLE_LDAP_SEARCH_FAILED)
    CASE(CURLE_OBSOLETE40)
    CASE(CURLE_FUNCTION_NOT_FOUND)
    CASE(CURLE_ABORTED_BY_CALLBACK)
    CASE(CURLE_BAD_FUNCTION_ARGUMENT)
    CASE(CURLE_OBSOLETE44)
    CASE(CURLE_INTERFACE_FAILED)
    CASE(CURLE_OBSOLETE46)
    CASE(CURLE_TOO_MANY_REDIRECTS)
    CASE(CURLE_UNKNOWN_OPTION)
    CASE(CURLE_TELNET_OPTION_SYNTAX)
    CASE(CURLE_OBSOLETE50)
    CASE(CURLE_PEER_FAILED_VERIFICATION)
    CASE(CURLE_GOT_NOTHING)
    CASE(CURLE_SSL_ENGINE_NOTFOUND)
    CASE(CURLE_SSL_ENGINE_SETFAILED)
    CASE(CURLE_SEND_ERROR)
    CASE(CURLE_RECV_ERROR)
    CASE(CURLE_OBSOLETE57)
    CASE(CURLE_SSL_CERTPROBLEM)
    CASE(CURLE_SSL_CIPHER)
    CASE(CURLE_SSL_CACERT)
    CASE(CURLE_BAD_CONTENT_ENCODING)
    CASE(CURLE_LDAP_INVALID_URL)
    CASE(CURLE_FILESIZE_EXCEEDED)
    CASE(CURLE_USE_SSL_FAILED)
    CASE(CURLE_SEND_FAIL_REWIND)
    CASE(CURLE_SSL_ENGINE_INITFAILED)
    CASE(CURLE_LOGIN_DENIED)
    CASE(CURLE_TFTP_NOTFOUND)
    CASE(CURLE_TFTP_PERM)
    CASE(CURLE_REMOTE_DISK_FULL)
    CASE(CURLE_TFTP_ILLEGAL)
    CASE(CURLE_TFTP_UNKNOWNID)
    CASE(CURLE_REMOTE_FILE_EXISTS)
    CASE(CURLE_TFTP_NOSUCHUSER)
    CASE(CURLE_CONV_FAILED)
    CASE(CURLE_CONV_REQD)
    CASE(CURLE_SSL_CACERT_BADFILE)
    CASE(CURLE_REMOTE_FILE_NOT_FOUND)
    CASE(CURLE_SSH)
    CASE(CURLE_SSL_SHUTDOWN_FAILED)
    CASE(CURLE_AGAIN)
    CASE(CURLE_SSL_CRL_BADFILE)
    CASE(CURLE_SSL_ISSUER_ERROR)
    CASE(CURLE_FTP_PRET_FAILED)
    CASE(CURLE_RTSP_CSEQ_ERROR)
    CASE(CURLE_RTSP_SESSION_ERROR)
    CASE(CURLE_FTP_BAD_FILE_LIST)
    CASE(CURLE_CHUNK_FAILED)
    CASE(CURLE_NO_CONNECTION_AVAILABLE)
    CASE(CURLE_SSL_PINNEDPUBKEYNOTMATCH)
    CASE(CURLE_SSL_INVALIDCERTSTATUS)
    default:
      return SUCCESS;
  };
#undef CASE
}

result print_http_error(const http::status error) {
#define CASE(ERROR)                                                  \
  case http::code::ERROR:                                            \
    fprintf(stderr, "HTTP request failed with error: %s\n", #ERROR); \
    return FAILURE;
  switch (error) {
    CASE(CONTINUE)
    CASE(SWITCHING_PROTOCOLS)
    CASE(PROCESSING)
    CASE(OK)
    CASE(CREATED)
    CASE(ACCEPTED)
    CASE(NON_AUTHORITATIVE_INFORMATION)
    CASE(NO_CONTENT)
    CASE(RESET_CONTENT)
    CASE(PARTIAL_CONTENT)
    CASE(MULTI_STATUS)
    CASE(ALREADY_REPORTED)
    CASE(IM_USED)
    CASE(MULTIPLE_CHOICES)
    CASE(MOVED_PERMANENTLY)
    CASE(FOUND)
    CASE(SEE_OTHER)
    CASE(NOT_MODIFIED)
    CASE(USE_PROXY)
    CASE(SWITCH_PROXY)
    CASE(TEMPORARY_REDIRECT)
    CASE(PERMANENT_REDIRECT)
    CASE(BAD_REQUEST)
    CASE(UNAUTHORIZED)
    CASE(PAYMENT_REQUIRED)
    CASE(FORBIDDEN)
    CASE(NOT_FOUND)
    CASE(NOT_ALLOWED)
    CASE(NOT_ACCEPTABLE)
    CASE(PROXY_AUTHENTICATION_REQUIRED)
    CASE(REQUEST_TIMEOUT)
    CASE(CONFLICT)
    CASE(GONE)
    CASE(LENGTH_REQUIRED)
    CASE(PRECONDITION_FAILED)
    CASE(PAYLOAD_TOO_LARGE)
    CASE(REQUEST_URI_TOO_LONG)
    CASE(UNSUPPORTED_MEDIA_TYPE)
    CASE(REQUESTED_RANGE_NOT_SATISFIABLE)
    CASE(EXPECTATION_FAILED)
    CASE(IM_A_TEAPOT)
    CASE(AUTHENTICATION_TIMEOUT)
    CASE(METHOD_FAILURE)
    CASE(MISDIRECTION_REQUEST)
    CASE(UNPROCESSABLE_ENTITY)
    CASE(LOCKED)
    CASE(FAILED_DEPENDENCY)
    CASE(UPGRADE_REQUIRED)
    CASE(PRECONDITION_REQUIRED)
    CASE(TOO_MANY_REQUESTS)
    CASE(REQUEST_HEADER_FIELDS_TOO_LARGE)
    CASE(LOGIN_TIMEOUT)
    CASE(NO_RESPONSE)
    CASE(RETRY_WITH)
    CASE(BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS)
    CASE(REDIRECT)
    CASE(REQUEST_HEADER_TOO_LARGE)
    CASE(CERT_ERROR)
    CASE(NO_CERT)
    CASE(HTTP_TO_HTTPS)
    CASE(TOKEN_EXPIRED_INVALID)
    CASE(CLIENT_CLOSED_REQUEST)
    CASE(INTERNAL_SERVER_ERROR)
    CASE(NOT_IMPLEMENTED)
    CASE(BAD_GATEWAY)
    CASE(SERVICE_UNAVAILABLE)
    CASE(GATEWAY_TIMEOUT)
    CASE(HTTP_VERSION_NOT_SUPPORTED)
    CASE(VARIANT_ALSO_NEGOTIATES)
    CASE(INSUFFICIENT_STORAGE)
    CASE(LOOP_DETECTION)
    CASE(BANDWIDTH_LIMIT_EXCEEDED)
    CASE(NOT_EXTENDED)
    CASE(NETWORK_AUTHENTICATION_REQUIRED)
    CASE(UNKNOWN_ERROR)
    CASE(ORIGIN_CONNECTION_TIMEOUT)
    CASE(NETWORK_READ_TIMEOUT_ERROR)
    CASE(NETWORK_CONNECT_TIMEOUT_ERROR)
    default:
      return SUCCESS;
  }
#undef CASE
}
}  // redmine
