#include "minhttp.h"

#define GO_TO_FIRST_NON_WHITESPACE(data, len, cursor) do {\
  for(; *cursor < len && data[*cursor] == '\n' && data[*cursor] == ' ' && data[*cursor] == '\r'; (*cursor)++);\
} while(0)

#define MIN(a, b) (a < b ? a : b)
/*
If 'method' is NULL, will return cursor pointed to first endpoint section byte.
*/
enum HTTP_PARSE_CODE METHOD_PREFIX parse_method(char* data, unsigned short len, unsigned short* cursor, enum HTTP_METHOD* method) {
  if(len <= *cursor) return DATA_LEN_TOO_SMALL;
  len -= *cursor;
  if(len < 3) return DATA_LEN_TOO_SMALL;
  if(!method) {
    for(;*cursor < len && data[*cursor] >= 'A' && data[*cursor] <= 'Z'; (*cursor)++);
    if(len <= *cursor) return DATA_LEN_TOO_SMALL;
    return SUCCESS;
  }
  switch(data[(*cursor)]) {
    // GET
    case 'G': {
      if(data[(*cursor)+1] != 'E' || data[(*cursor)+2] != 'T') return INVALID_DATA;
      *method = GET;
      *cursor += 3;
      return SUCCESS;
    }
    // PUT
    // POST
    // PATCH
    case 'P': {
      switch(data[(*cursor)+1]) {
        case 'U': {
          if(data[(*cursor)+2] != 'T') return INVALID_DATA;
          *method = PUT;
          *cursor += 3;
          return SUCCESS;
        }
        case 'O': {
          if(len < 4) return DATA_LEN_TOO_SMALL;
          if(data[(*cursor)+2] != 'S' || data[(*cursor)+3] != 'T') return INVALID_DATA;
          *method = POST;
          *cursor += 4;
          return SUCCESS;
        }
        case 'A': {
          if(len < 5) return DATA_LEN_TOO_SMALL;
          if(data[(*cursor)+2] != 'T' || data[(*cursor)+3] != 'C' || data[(*cursor)+4] != 'H') return INVALID_DATA;
          *method = PATCH;
          *cursor += 5;
          return SUCCESS;
        }
      }
    }
    // OPTIONS
    case 'O': {
      if(len < 7) return DATA_LEN_TOO_SMALL;
      if(data[(*cursor)+1] != 'P' ||
        data[(*cursor)+2] != 'T' ||
        data[(*cursor)+3] != 'I' ||
        data[(*cursor)+4] != 'O' ||
        data[(*cursor)+5] != 'N' ||
        data[(*cursor)+6] != 'S') return INVALID_DATA;
      *method = OPTIONS;
      *cursor += 7;
      return SUCCESS;
    }
    // HEAD
    case 'H': {
      if(len < 4) return DATA_LEN_TOO_SMALL;
      if(data[(*cursor)+1] != 'E' || data[(*cursor)+2] != 'A' || data[(*cursor)+3] != 'D') return INVALID_DATA;
      *method = HEAD;
      *cursor += 4;
      return SUCCESS;
    }
    // DELETE
    case 'D': {
      if(len < 6) return DATA_LEN_TOO_SMALL;
      if(data[(*cursor)+1] != 'E' ||
          data[(*cursor)+2] != 'L' ||
          data[(*cursor)+3] != 'E' ||
          data[(*cursor)+4] != 'T' ||
          data[(*cursor)+5] != 'E') return INVALID_DATA;
      *method = DELETE;
      *cursor += 6;
      return SUCCESS;
    }
    // TRACE
    case 'T': {
      if(len < 5) return DATA_LEN_TOO_SMALL;
      if(data[(*cursor)+1] != 'R' ||
          data[(*cursor)+2] != 'A' ||
          data[(*cursor)+3] != 'C' ||
          data[(*cursor)+4] != 'E') return INVALID_DATA;
      *method = TRACE;
      *cursor += 5;
      return SUCCESS;
    }
  }
  return INVALID_DATA;
}
/*
'parse_method' must be called first. 
If 'endpoint' is NULL, will still return cursor pointed to first byte after endpoint part.
After the operation, 'max_len' will contain the 'endpoint' len.
Will return 'ENDPOINT_TRUNCATED' if 'max_len' wasn't enough to copy endpoint
*/
enum HTTP_PARSE_CODE METHOD_PREFIX parse_endpoint(char* data, unsigned short len, unsigned short* cursor, char* endpoint, unsigned short* max_len) {
  // len -= *cursor;
  // len = MIN(len, *max_len);
  // for(; *cursor < len && *cursor < *max_len && data[*cursor] = ; (*cursor)++) {
  //
  // }
  // if(*cursor >= *max_len) {
  //   return ENDPOINT_TRUNCATED;
  // }
  return SUCCESS;
}
/*
'parse_parse_endpoint' must be called first. 
If 'version' is NULL, will return cursor pointed to first header section byte.
*/
enum HTTP_PARSE_CODE METHOD_PREFIX parse_http_version(char* data, unsigned short len, unsigned short* cursor, enum HTTP_VERSION* version) {}
/*
'parse_http_version' must be called first. 
If 'headers' is NULL, will return cursor pointed to first payload byte.
'headers' must be an array of arrays, where each element has a key header as the first item and a writable address of 'max_value_len' as the second.
*/
enum HTTP_PARSE_CODE METHOD_PREFIX parse_headers(char* data, unsigned short len, unsigned short* cursor, char** headers, unsigned short num_headers, unsigned short max_key_len, unsigned short max_value_len) {}

enum HTTP_PARSE_CODE METHOD_PREFIX parse_http(
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
) {
  if(!len) return NULL_LEN;
  unsigned short cursor = 0;
  GO_TO_FIRST_NON_WHITESPACE(data, *len, &cursor);
  if(*len <= cursor) return DATA_LEN_TOO_SMALL;
  enum HTTP_PARSE_CODE result;
  if(( result = parse_method(data, *len, &cursor, method) )) goto _return_result;


_return_result:
  return result;
}
