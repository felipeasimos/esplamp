#pragma once

#include <stdint.h>

typedef enum MH_HTTP_METHOD {
  GET = 1,
  POST = 2,
  PUT = 3,
  PATCH = 4,
  OPTIONS = 5,
  HEAD = 6,
  DELETE = 7,
  TRACE = 8,
  CONNECT = 9
} mh_method;

typedef enum MH_HTTP_VERSION {
  HTTP_1 = 1,
  HTTP_1_1 = 2,
} mh_version;

typedef struct MH_HTTP_HEADER {
  char* header_key_begin;
  uint16_t header_key_len;
  char* header_value_begin;
  uint16_t header_value_len;
} mh_header;

/* 
char* data - pointer to first char of data to parse
char* data_end - pointer to final char of data + 1 to parse
mh_method* method - where the method value will be written to.
const char* path - where the path will be written to.
uint32_t* path_len - maximum length of the path pointer allocated memory. After function execution will be the value of bytes written to it.
version - where the version value will be written to.
return:
  - NULL -> error
  - address -> address after newline
*/
char* mh_parse_request_first_line(char* data, char* data_end, char** method, uint8_t* method_len, char** path, uint32_t* path_len, mh_version* version);

/* 
char* data - pointer to first char of data to parse
char* data_end - pointer to final char of data + 1 to parse
mh_version* version - where the version will be written to.
uint16_t* status - where the status code will be written to.
char* phrase - where the reason phrase will be written to.
uint32_t* phrase_len - maximum length of the phrase pointer allocated memory. After function execution will be the value of bytes written to it.
return:
  - NULL -> error
  - address -> address after newline
*/
char* mh_parse_response_first_line(char* data, char* data_end, mh_version* version, uint16_t* status, char** phrase, uint32_t* phrase_len);

/*
char* data - pointer to first char of data to parse
char* data_end - pointer to final char of data + 1 to parse
mh_header* headers - pointer to array of mh_header structs to be written to
uint32_t* num_headers - number of headers in array. Will return the number of mh_headers written to
 */
char* mh_parse_headers(char* data, char* data_end, mh_header* headers, uint32_t* num_headers);

/*
char* data - pointer to first char of data to parse
char* data_end - pointer to final char of data + 1 to parse
mh_header* headers - pointer to array of mh_header structs to be written to. 'key_begin' and 'key_len' must already be filled
uint32_t* num_headers - number of headers in array. Will return the number of mh_headers written to
 */
char* mh_parse_headers_set(char* data, char* data_end, mh_header* headers, uint32_t num_headers);
