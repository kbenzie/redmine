#include <request.h>

#include <curl/curl.h>

result_t curl_print_error(CURLcode error, const char *file, const int line);
#define CURL_CHECK_RETURN(EXPRESSION)                                      \
  if (result_t error = curl_print_error(EXPRESSION, __FILE__, __LINE__)) { \
    return error;                                                          \
  }

size_t write(void *ptr, size_t size, size_t count, void *stream) {
  std::stringstream &out = *static_cast<std::stringstream *>(stream);
  const size_t written = size * count;
  out.write(static_cast<char *>(ptr), written);
  return written;
}

result_t request(const char *url, const char *key, options_t options,
                 std::stringstream &body) {
  CURL_CHECK_RETURN(curl_global_init(CURL_GLOBAL_ALL));
  CURL *curl = curl_easy_init();
  CHECK(!curl, fprintf(stderr, "curl_easy_init failed\n"); return FAILURE);
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_URL, url));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write));
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body));

  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_PORT, 443));

  if (has_opt<VERBOSE>(options)) {
    CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_VERBOSE, true));
    CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_HEADER, true));
  }

  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false));

  struct curl_slist *header = nullptr;
  std::string api_key_header("X-Redmine-API-Key: ");
  api_key_header += key;
  header = curl_slist_append(header, api_key_header.c_str());
  header = curl_slist_append(header, "Content-Type: application/json");
  CURL_CHECK_RETURN(curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header));

  CURL_CHECK_RETURN(curl_easy_perform(curl));
  uint32_t response;
  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response);

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  CHECK(http::OK != response,
        fprintf(stderr, "HTTP request failed with error: %u\n", response);
        return FAILURE);

  if (has_opt<VERBOSE>(options)) {
    printf("body: %s\n", body.str().c_str());
  }

  return SUCCESS;
}

result_t curl_print_error(CURLcode error, const char *file, const int line) {
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
