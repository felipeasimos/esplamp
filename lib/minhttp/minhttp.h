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
  NULL_CURSOR,
  DATA_LEN_TOO_SMALL,
  INVALID_DATA
};

#ifdef ICACHE_FLASH_ATTR
  #define METHOD_PREFIX ICACHE_FLASH_ATTR
#else
  #define METHOD_PREFIX 
#endif

/*
If 'method' is NULL, will still return cursor pointed to first byte after method part.
'cursor' value only changes on success.
*/
enum HTTP_PARSE_CODE ICACHE_FLASH_ATTR parse_method(char* data, unsigned short len, unsigned short* cursor, enum HTTP_METHOD* method);
/*
'parse_method' must be called first. 
If 'endpoint' is NULL, will still return cursor pointed to first byte after endpoint part.
'cursor' value only changes on success.
*/
enum HTTP_PARSE_CODE ICACHE_FLASH_ATTR parse_endpoint(char* data, unsigned short len, unsigned short* cursor, char* endpoint, unsigned short max_len);
/*
'parse_parse_endpoint' must be called first. 
If 'version' is NULL, will still return cursor pointed to first byte after version part (newline).
'cursor' value only changes on success.
*/
enum HTTP_PARSE_CODE ICACHE_FLASH_ATTR parse_http_version(char* data, unsigned short len, unsigned short* cursor, enum HTTP_VERSION* version);
/*
'parse_http_version' must be called first. 
If 'headers' is NULL, will still return cursor pointed to first byte after header section (invalid, or double newline).
'headers' must be an array of arrays, where each element has a key header as the first item and a writable address of 'max_value_len' as the second.
'cursor' value only changes on success.
*/
enum HTTP_PARSE_CODE ICACHE_FLASH_ATTR parse_headers(char* data, unsigned short len, unsigned short* cursor, char** headers, unsigned short num_headers, unsigned short max_key_len, unsigned short max_value_len);


