#include "c_types.h"

enum HTTP_METHOD {
  GET = 1,
  POST = 2,
  PUT = 3,
  PATCH = 4,
  OPTIONS = 5,
  HEAD = 6,
  DELETE = 7,
  TRACE = 8
};

enum HTTP_VERSION {
  HTTP_1 = 1,
  HTTP_1_1 = 2,
  HTTP_2 = 3,
  HTTP_3 = 4
};

enum HTTP_PARSE_CODE {
  SUCCESS = 0,
  NULL_LEN,
  DATA_LEN_TOO_SMALL,
  INVALID_DATA,
  ENDPOINT_TRUNCATED
};

#ifdef ICACHE_FLASH_ATTR
  #define METHOD_PREFIX ICACHE_FLASH_ATTR
#else
  #define METHOD_PREFIX 
#endif

/*
parse http request.
'data' is your request string.
'len' is its size (or max size). As a return value, its the index in 'data' for the first payload byte.
'method' will return the parsed HTTP_METHOD.
'endpoint' will return the endpoint parsed. It must point to an allocated space of at least 'max_endpoint_len' bytes.
'version' will return an http version.
'headers' is a 2 dimensional array, where each element contains two items: an already defined HTTP header and an address where the value can be stored. The function will return the values of the defined keys in these addresses, halting parsing for a header if it hits 'max_key_len' and truncating values with 'max_value_len'. Keys that are not found in the given 'headers' will be ignored. The address used to store they keys must start with an NULL byte. Headers with empty keys are not parsed. Duplicate parser keys have only the first value saved.
'method', 'endpoint', 'version' and 'headers' can be NULL. The parsing will proceed faster in the NULLified sections, by just looking for the section.
 */
enum HTTP_PARSE_CODE METHOD_PREFIX parse_http_request(
    char* data,
    unsigned short* len,
    enum HTTP_METHOD* method,
    char* endpoint,
    unsigned short* max_endpoint_len,
    enum HTTP_VERSION* version,
    char** headers,
    unsigned short num_headers,
    unsigned short max_key_len,
    unsigned short max_value_len
);

void METHOD_PREFIX build_http_request();

enum HTTP_PARSE_CODE METHOD_PREFIX parse_http_response();

/*
returns HTTP response in 'data' with length 'len'
 */
void METHOD_PREFIX build_http_response(
    char* data,
    unsigned short* len,
    enum HTTP_METHOD method,
    char* endpoint,
    enum HTTP_VERSION version,
    char** headers,
    unsigned short num_headers,
    unsigned short max_key_len,
    unsigned short max_value_len,
    void* payload,
    unsigned short payload_len
);
