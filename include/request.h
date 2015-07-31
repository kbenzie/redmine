#ifndef REQUEST_H
#define REQUEST_H

#include <redmine.h>

#include <string>

/// @brief Set of all HTTP status code values.
struct http {
  /// @brief Continue informational status code.
  ///
  /// This means that the server has received the request headers, and that the
  /// client should proceed to send the request body (in the case of a request
  /// for which a body needs to be sent; for example, a POST request). If the
  /// request body is large, sending it to a server when a request has already
  /// been rejected based upon inappropriate headers is inefficient. To have a
  /// server check if the request could be accepted based on the request's
  /// headers alone, a client must send Expect: 100-continue as a header in its
  /// initial request and check if a 100 Continue status code is received in
  /// response before continuing (or receive 417 Expectation Failed and not
  /// continue).
  static const uint32_t CONTINUE = 100;

  /// @brief Switching Protocols informational status code.
  ///
  /// This means the requester has asked the server to switch protocols and the
  /// server is acknowledging that it will do so.
  static const uint32_t SWITCHING_PROTOCOLS = 101;

  /// @brief Processing (WebDAV; RFC 2518) informational status code.
  ///
  /// This means the requester has asked the server to switch protocols and the
  /// server is acknowledging that it will do so.
  static const uint32_t PROCESSING = 102;

  /// @brief OK success status code.
  ///
  /// Standard response for successful HTTP requests. The actual response will
  /// depend on the request method used. In a GET request, the response will
  /// contain an entity corresponding to the requested resource. In a POST
  /// request, the response will contain an entity describing or containing the
  /// result of the action.
  static const uint32_t OK = 200;

  /// @brief Created success status code.
  ///
  /// The request has been fulfilled and resulted in a new resource being
  /// created.
  static const uint32_t CREATED = 201;

  /// @brief Accepted
  ///
  /// The request has been accepted for processing, but the processing has not
  /// been completed. The request might or might not eventually be acted upon,
  /// as it might be disallowed when processing actually takes place.
  static const uint32_t ACCEPTED = 202;

  /// @brief Non-Authoritative Information (since HTTP/1.1)
  ///
  /// The server successfully processed the request, but is returning
  /// information that may be from another source.
  static const uint32_t NON_AUTHORITATIVE_INFORMATION = 203;

  /// @brief No Content
  ///
  /// The server successfully processed the request, but is not returning any
  /// content. Usually used as a response to a successful delete request.
  static const uint32_t NO_CONTENT = 204;

  /// @brief Reset Content
  ///
  /// The server successfully processed the request, but is not returning any
  /// content. Unlike a 204 response, this response requires that the requester
  /// reset the document view.
  static const uint32_t RESET_CONTENT = 205;

  /// @brief Partial Content (RFC 7233)
  ///
  /// The server is delivering only part of the resource (byte serving) due to a
  /// range header sent by the client. The range header is used by tools like
  /// wget to enable resuming of interrupted downloads, or split a download into
  /// multiple simultaneous streams.
  static const uint32_t PARTIAL_CONTENT = 206;

  /// @brief Multi-Status (WebDAV; RFC 4918)
  ///
  /// The message body that follows is an XML message and can contain a number
  /// of separate response codes, depending on how many sub-requests were
  /// made.
  static const uint32_t MULTI_STATUS = 207;

  /// @brief Already Reported (WebDAV; RFC 5842)
  ///
  /// The members of a DAV binding have already been enumerated in a previous
  /// reply to this request, and are not being included again.
  static const uint32_t ALREADY_REPORTED = 208;

  /// @brief IM Used (RFC 3229)
  ///
  /// The server has fulfilled a request for the resource, and the response is a
  /// representation of the result of one or more instance-manipulations applied
  /// to the current instance.
  static const uint32_t IM_USED = 226;

  /// @brief Multiple Choices redirection status code.
  ///
  /// Indicates multiple options for the resource that the client may follow.
  /// It, for instance, could be used to present different format options for
  /// video, list files with different extensions, or word sense disambiguation.
  static const uint32_t MULTIPLE_CHOICES = 300;

  /// @brief Moved Permanently redirection status code.
  ///
  /// This and all future requests should be directed to the given URI.
  static const uint32_t MOVED_PERMANENTLY = 301;

  /// @brief Found redirection status code.
  ///
  /// This is an example of industry practice contradicting the standard. The
  /// HTTP/1.0 specification (RFC 1945) required the client to perform a
  /// temporary redirect (the original describing phrase was "Moved
  /// Temporarily"), but popular browsers implemented 302 with the
  /// functionality of a 303 See Other. Therefore, HTTP/1.1 added status codes
  /// 303 and 307 to distinguish between the two behaviours. However, some
  /// Web applications and frameworks use the 302 status code as if it were the
  /// 303.
  static const uint32_t FOUND = 302;

  /// @brief See Other (since HTTP/1.1) redirection status code.
  ///
  /// The response to the request can be found under another URI using a GET
  /// method. When received in response to a POST (or PUT/DELETE), it should be
  /// assumed that the server has received the data and the redirect should be
  /// issued with a separate GET message.
  static const uint32_t SEE_OTHER = 303;

  /// @brief Not Modified (RFC 7232) redirection status code.
  ///
  /// Indicates that the resource has not been modified since the version
  /// specified by the request headers If-Modified-Since or If-None-Match. This
  /// means that there is no need to retransmit the resource, since the client
  /// still has a previously-downloaded copy.
  static const uint32_t NOT_MODIFIED = 304;

  /// @brief Use Proxy (since HTTP/1.1) redirection status code.
  ///
  /// The requested resource is only available through a proxy, whose address is
  /// provided in the response. Many HTTP clients (such as Mozilla and
  /// Internet Explorer) do not correctly handle responses with this status
  /// code, primarily for security reasons.
  static const uint32_t USE_PROXY = 305;

  /// @brief Switch Proxy redirection status code.
  ///
  /// No longer used. Originally meant "Subsequent requests should use the
  /// specified proxy."
  static const uint32_t SWITCH_PROXY = 306;

  /// @brief Temporary Redirect (since HTTP/1.1) redirection status code.
  ///
  /// In this case, the request should be repeated with another URI; however,
  /// future requests should still use the original URI. In contrast to how 302
  /// was historically implemented, the request method is not allowed to be
  /// changed when reissuing the original request. For instance, a POST request
  /// should be repeated using another POST request.
  static const uint32_t TEMPORARY_REDIRECT = 307;

  /// @brief Permanent Redirect (RFC 7538) redirection status code.
  ///
  /// The request, and all future requests should be repeated using another URI.
  /// 307 and 308 (as proposed) parallel the behaviours of 302 and 301, but do
  /// not allow the HTTP method to change. So, for example, submitting a form to
  /// a permanently redirected resource may continue smoothly.
  static const uint32_t PERMANENT_REDIRECT = 308;

  /// @brief Bad Request client error status code.
  ///
  /// The server cannot or will not process the request due to something that is
  /// perceived to be a client error (e.g., malformed request syntax, invalid
  /// request message framing, or deceptive request routing).
  static const uint32_t BAD_REQUEST = 400;

  /// @brief Unauthorized (RFC 7235) client error status code.
  ///
  /// Similar to 403 Forbidden, but specifically for use when authentication is
  /// required and has failed or has not yet been provided. The response must
  /// include a WWW-Authenticate header field containing a challenge applicable
  /// to the requested resource. See Basic access authentication and Digest
  /// access authentication.
  static const uint32_t UNAUTHORIZED = 401;

  /// @brief Payment Required client error status code.
  ///
  /// Reserved for future use. The original intention was that this code might
  /// be used as part of some form of digital cash or micropayment scheme, but
  /// that has not happened, and this code is not usually used. YouTube uses
  /// this status if a particular IP address has made excessive requests, and
  /// requires the person to enter a CAPTCHA.
  static const uint32_t PAYMENT_REQUIRED = 402;

  /// @brief Forbidden client error status code.
  ///
  /// The request was a valid request, but the server is refusing to respond to
  /// it. Unlike a 401 Unauthorized response, authenticating will make no
  /// difference.
  static const uint32_t FORBIDDEN = 403;

  /// @brief Not Found client error status code.
  ///
  /// The requested resource could not be found but may be available again in
  /// the future. Subsequent requests by the client are permissible.
  static const uint32_t NOT_FOUND = 404;

  /// @brief Method Not Allowed client error status code.
  ///
  /// A request was made of a resource using a request method not supported by
  /// that resource; for example, using GET on a form which requires data to be
  /// presented via POST, or using PUT on a read-only resource.
  static const uint32_t NOT_ALLOWED = 405;

  /// @brief Not Acceptable client error status code.
  ///
  /// The requested resource is only capable of generating content not
  /// acceptable according to the Accept headers sent in the request.
  static const uint32_t NOT_ACCEPTABLE = 406;

  /// @brief Proxy Authentication Required (RFC 7235) client error status code.
  ///
  /// The client must first authenticate itself with the proxy.
  static const uint32_t PROXY_AUTHENTICATION_REQUIRED = 407;

  /// @brief Request Timeout client error status code.
  ///
  /// The server timed out waiting for the request. According to HTTP
  /// specifications: "The client did not produce a request within the time that
  /// the server was prepared to wait. The client MAY repeat the request without
  /// modifications at any later time."
  static const uint32_t REQUEST_TIMEOUT = 408;

  /// @brief Conflict client error status code.
  ///
  /// Indicates that the request could not be processed because of conflict in
  /// the request, such as an edit conflict in the case of multiple updates.
  static const uint32_t CONFLICT = 409;

  /// @brief Gone client error status code.
  ///
  /// Indicates that the resource requested is no longer available and will not
  /// be available again. This should be used when a resource has been
  /// intentionally removed and the resource should be purged. Upon receiving a
  /// 410 status code, the client should not request the resource again in the
  /// future. Clients such as search engines should remove the resource from
  /// their indices. Most use cases do not require clients and search
  /// engines to purge the resource, and a "404 Not Found" may be used instead.
  static const uint32_t GONE = 410;

  /// @brief Length Required client error status code.
  ///
  /// The request did not specify the length of its content, which is required
  /// by the requested resource.
  static const uint32_t LENGTH_REQUIRED = 411;

  /// @brief Precondition Failed (RFC 7232) client error status code.
  ///
  /// The server does not meet one of the preconditions that the requester put
  /// on the request.
  static const uint32_t PRECONDITION_FAILED = 412;

  /// @brief Payload Too Large (RFC 7231) client error status code.
  ///
  /// The request is larger than the server is willing or able to process.
  /// Called "Request Entity Too Large " previously.
  static const uint32_t PAYLOAD_TOO_LARGE = 413;

  /// @brief Request-URI Too Long client error status code.
  ///
  /// The URI provided was too long for the server to process. Often the result
  /// of too much data being encoded as a query-string of a GET request, in
  /// which case it should be converted to a POST request.
  static const uint32_t REQUEST_URI_TOO_LONG = 414;

  /// @brief Unsupported Media Type client error status code.
  ///
  /// The request entity has a media type which the server or resource does not
  /// support. For example, the client uploads an image as image/svg+xml, but
  /// the server requires that images use a different format.
  static const uint32_t UNSUPPORTED_MEDIA_TYPE = 415;

  /// @brief Requested Range Not Satisfiable (RFC 7233) client error status
  /// code.
  ///
  /// The client has asked for a portion of the file (byte serving), but the
  /// server cannot supply that portion. For example, if the client asked for a
  /// part of the file that lies beyond the end of the file.
  static const uint32_t REQUESTED_RANGE_NOT_SATISFIABLE = 416;

  /// @brief Expectation Failed client error status code.
  ///
  /// The server cannot meet the requirements of the Expect request-header
  /// field.
  static const uint32_t EXPECTATION_FAILED = 417;

  /// @brief I'm a teapot (RFC 2324) client error status code.
  ///
  /// This code was defined in 1998 as one of the traditional IETF April Fools'
  /// jokes, in RFC 2324, Hyper Text Coffee Pot Control Protocol, and is not
  /// expected to be implemented by actual HTTP servers. The RFC specifies this
  /// code should be returned by tea pots requested to brew coffee.
  static const uint32_t IM_A_TEAPOT = 418;

  /// @brief Authentication Timeout (not in RFC 2616) client error status code.
  ///
  /// Not a part of the HTTP standard, 419 Authentication Timeout denotes that
  /// previously valid authentication has expired. It is used as an alternative
  /// to 401 Unauthorized in order to differentiate from otherwise authenticated
  /// clients being denied access to specific server resources.
  static const uint32_t AUTHENTICATION_TIMEOUT = 419;

  /// @brief Method Failure (Spring Framework) client error status code.
  ///
  /// Not part of the HTTP standard, but defined by Spring in the HttpStatus
  /// class to be used when a method failed. This status code is deprecated by
  /// Spring.
  static const uint32_t METHOD_FAILURE = 420;

  /// @brief Enhance Your Calm (Twitter) client error status code.
  ///
  /// Not part of the HTTP standard, but returned by version 1 of the Twitter
  /// Search and Trends API when the client is being rate limited. Other
  /// services may wish to implement the 429 Too Many Requests response code
  /// instead.
  static const uint32_t ENHANCE_YOUR_CALM = 420;

  /// @brief Misdirected Request (HTTP/2) client error status code.
  ///
  /// The request was directed at a server that is not able to produce a
  /// response (for example because a connection reuse).
  static const uint32_t MISDIRECTION_REQUEST = 421;

  /// @brief Unprocessable Entity (WebDAV; RFC 4918) client error status code.
  ///
  /// The request was well-formed but was unable to be followed due to semantic
  /// errors.
  static const uint32_t UNPROCESSABLE_ENTITY = 422;

  /// @brief Locked (WebDAV; RFC 4918) client error status code.
  ///
  /// The resource that is being accessed is locked.
  static const uint32_t LOCKED = 423;

  /// @brief Failed Dependency (WebDAV; RFC 4918) client error status code.
  ///
  /// The request failed due to failure of a previous request (e.g., a
  /// PROPPATCH).
  static const uint32_t FAILED_DEPENDENCY = 424;

  /// @brief Upgrade Required client error status code.
  ///
  /// The client should switch to a different protocol such as TLS/1.0, given in
  /// the Upgrade header field.
  static const uint32_t UPGRADE_REQUIRED = 426;

  /// @brief Precondition Required (RFC 6585) client error status code.
  ///
  /// The origin server requires the request to be conditional. Intended to
  /// prevent "the 'lost update' problem, where a client GETs a resource's
  /// state, modifies it, and PUTs it back to the server, when meanwhile a third
  /// party has modified the state on the server, leading to a conflict."
  static const uint32_t PRECONDITION_REQUIRED = 428;

  /// @brief Too Many Requests (RFC 6585) client error status code.
  ///
  /// The user has sent too many requests in a given amount of time. Intended
  /// for use with rate limiting schemes.
  static const uint32_t TOO_MANY_REQUESTS = 429;

  /// @brief Request Header Fields Too Large (RFC 6585) client error status
  /// code.
  ///
  /// The server is unwilling to process the request because either an
  /// individual header field, or all the header fields collectively, are too
  /// large.
  static const uint32_t REQUEST_HEADER_FIELDS_TOO_LARGE = 431;

  /// @brief Login Timeout (Microsoft) client error status code.
  ///
  /// A Microsoft extension. Indicates that your session has expired.
  static const uint32_t LOGIN_TIMEOUT = 440;

  /// @brief No Response (Nginx) client error status code.
  ///
  /// Used in Nginx logs to indicate that the server has returned no information
  /// to the client and closed the connection (useful as a deterrent for
  /// malware).
  static const uint32_t NO_RESPONSE = 444;

  /// @brief Retry With (Microsoft) client error status code.
  ///
  /// A Microsoft extension. The request should be retried after performing the
  /// appropriate action.
  static const uint32_t RETRY_WITH = 449;

  /// @brief Blocked by Windows Parental Controls (Microsoft) client error
  /// status code.
  ///
  /// A Microsoft extension. This error is given when Windows Parental Controls
  /// are turned on and are blocking access to the given webpage.
  static const uint32_t BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS = 450;

  /// @brief Unavailable For Legal Reasons (Internet draft) client error status
  /// code.
  ///
  /// Defined in the internet draft "A New HTTP Status Code for
  /// Legally-restricted Resources". Intended to be used when resource
  /// access is denied for legal reasons, e.g. censorship or government-mandated
  /// blocked access. A reference to the 1953 dystopian novel Fahrenheit 451,
  /// where books are outlawed.
  static const uint32_t UNAVAILABLE_FOR_LEGAL_REASONS = 451;

  /// @brief Redirect (Microsoft) client error status code.
  ///
  /// Used in Exchange ActiveSync if there either is a more efficient server to
  /// use or the server cannot access the users' mailbox.
  /// The client is supposed to re-run the HTTP Autodiscovery protocol to find a
  /// better suited server.
  static const uint32_t REDIRECT = 451;

  /// @brief Request Header Too Large (Nginx) client error status code.
  ///
  /// Nginx internal code similar to 431 but it was introduced earlier in
  /// version 0.9.4 (on January 21, 2011).
  static const uint32_t REQUEST_HEADER_TOO_LARGE = 494;

  /// @brief Cert Error (Nginx) client error status code.
  ///
  /// Nginx internal code used when SSL client certificate error occurred to
  /// distinguish it from 4XX in a log and an error page redirection.
  static const uint32_t CERT_ERROR = 495;

  /// @brief No Cert (Nginx) client error status code.
  ///
  /// Nginx internal code used when client didn't provide certificate to
  /// distinguish it from 4XX in a log and an error page redirection.
  static const uint32_t NO_CERT = 496;

  /// @brief HTTP to HTTPS (Nginx) client error status code.
  ///
  /// Nginx internal code used for the plain HTTP requests that are sent to
  /// HTTPS port to distinguish it from 4XX in a log and an error page
  /// redirection.
  static const uint32_t HTTP_TO_HTTPS = 497;

  /// @brief Token expired/invalid (Esri) client error status code.
  ///
  /// Returned by ArcGIS for Server. A code of 498 indicates an expired or
  /// otherwise invalid token.
  static const uint32_t TOKEN_EXPIRED_INVALID = 498;

  /// @brief Client Closed Request (Nginx) client error status code.
  ///
  /// Used in Nginx logs to indicate when the connection has been closed by
  /// client while the server is still processing its request, making server
  /// unable to send a status code back.
  static const uint32_t CLIENT_CLOSED_REQUEST = 499;

  /// @brief Token required (Esri) client error status code.
  ///
  /// Returned by ArcGIS for Server. A code of 499 indicates that a token is
  /// required (if no token was submitted).
  static const uint32_t TOKEN_REQUIRED = 499;

  /// @brief Internal Server Error server error status code.
  ///
  /// A generic error message, given when an unexpected condition was
  /// encountered and no more specific message is suitable.
  static const uint32_t INTERNAL_SERVER_ERROR = 500;

  /// @brief Not Implemented server error status code.
  ///
  /// The server either does not recognize the request method, or it lacks the
  /// ability to fulfill the request. Usually this implies future availability
  /// (e.g., a new feature of a web-service API).
  static const uint32_t NOT_IMPLEMENTED = 501;

  /// @brief Bad Gateway server error status code.
  ///
  /// The server was acting as a gateway or proxy and received an invalid
  /// response from the upstream server.
  static const uint32_t BAD_GATEWAY = 502;

  /// @brief Service Unavailable server error status code.
  ///
  /// The server is currently unavailable (because it is overloaded or down for
  /// maintenance). Generally, this is a temporary state.
  static const uint32_t SERVICE_UNAVAILABLE = 503;

  /// @brief Gateway Timeout server error status code.
  ///
  /// The server was acting as a gateway or proxy and did not receive a timely
  /// response from the upstream server.
  static const uint32_t GATEWAY_TIMEOUT = 504;

  /// @brief HTTP Version Not Supported server error status code.
  ///
  /// The server does not support the HTTP protocol version used in the request.
  static const uint32_t HTTP_VERSION_NOT_SUPPORTED = 505;

  /// @brief Variant Also Negotiates (RFC 2295) server error status code.
  ///
  /// Transparent content negotiation for the request results in a circular
  /// reference.
  static const uint32_t VARIANT_ALSO_NEGOTIATES = 506;

  /// @brief Insufficient Storage (WebDAV; RFC 4918) server error status code.
  ///
  /// The server is unable to store the representation needed to complete the
  /// request.
  static const uint32_t INSUFFICIENT_STORAGE = 507;

  /// @brief Loop Detected (WebDAV; RFC 5842) server error status code.
  ///
  /// The server detected an infinite loop while processing the request (sent in
  /// lieu of 208 Already Reported).
  static const uint32_t LOOP_DETECTION = 508;

  /// @brief Bandwidth Limit Exceeded (Apache bw/limited extension) server
  /// error status code.
  ///
  /// This status code is not specified in any RFCs. Its use is unknown.
  static const uint32_t BANDWIDTH_LIMIT_EXCEEDED = 509;

  /// @brief Not Extended (RFC 2774) server error status code.
  ///
  /// Further extensions to the request are required for the server to fulfil
  /// it.
  static const uint32_t NOT_EXTENDED = 510;

  /// @brief Network Authentication Required (RFC 6585) server error status
  /// code.
  ///
  /// The client needs to authenticate to gain network access. Intended for use
  /// by intercepting proxies used to control access to the network (e.g.,
  /// "captive portals" used to require agreement to Terms of Service before
  /// granting full Internet access via a Wi-Fi hotspot).
  static const uint32_t NETWORK_AUTHENTICATION_REQUIRED = 511;

  /// @brief Unknown Error server error status code.
  ///
  /// This status code is not specified in any RFC and is returned by certain
  /// services, for instance Microsoft Azure and CloudFlare servers: "The 520
  /// error is essentially a “catch-all” response for when the origin server
  /// returns something unexpected or something that is not
  /// tolerated/interpreted (protocol violation or empty response)."
  static const uint32_t UNKNOWN_ERROR = 520;

  /// @brief Origin Connection Time-out server error status code.
  ///
  /// This status code is not specified in any RFCs, but is used by CloudFlare's
  /// reverse proxies to signal that a server connection timed out.
  static const uint32_t ORIGIN_CONNECTION_TIMEOUT = 522;

  /// @brief Network read timeout error (Unknown) server error status code.
  ///
  /// This status code is not specified in any RFCs, but is used by Microsoft
  /// HTTP proxies to signal a network read timeout behind the proxy to a client
  /// in front of the proxy.
  static const uint32_t NETWORK_READ_TIMEOUT_ERROR = 598;

  /// @brief Network connect timeout error (Unknown) server error status code.
  ///
  /// This status code is not specified in any RFCs, but is used by Microsoft
  /// HTTP proxies to signal a network connect timeout behind the proxy to a
  /// client in front of the proxy.
  static const uint32_t NETWORK_CONNECT_TIMEOUT_ERROR = 599;
};

/// @brief Perform a URL request.
///
/// @param[in] url The URL to send the request to.
/// @param[in] key The users redmine API key.
/// @param[in] options Enabled options.
/// @param[out] body Response data body.
///
/// @return Return SUCCESS or FAILURE
result_t request(const char *url, const char *key, options_t options,
                 std::string &body);

#endif
