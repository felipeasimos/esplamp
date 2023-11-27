#include "minhttp.h"
#include "osapi.h"

#define GO_TO_FIRST_NON_WHITESPACE(data, len, cursor) do {\
  for(; *cursor < len && (data[*cursor] == '\n' || data[*cursor] == ' ' || data[*cursor] == '\r'); (*cursor)++);\
} while(0)

#define PARSE_NEWLINE(data, len, cursor) do {\
  switch(data[*cursor]) {\
    case '\n': {\
      (*cursor)++;\
      break;\
    }\
    case '\r': {\
      if(data[(*cursor)+1] != '\n') return INVALID_DATA;\
      (*cursor) += 2;\
      break;\
    }\
    default:\
      return INVALID_DATA;\
  }\
} while(0)

#define MIN(a, b) (a < b ? a : b)
/*
If 'method' is NULL, will return cursor pointed to first endpoint section byte.
*/
enum HTTP_PARSE_CODE METHOD_PREFIX parse_method(char* data, unsigned short len, unsigned short* cursor, enum HTTP_METHOD* method) {
  if(len <= *cursor || len - *cursor < 3) return DATA_LEN_TOO_SMALL;
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
  if(len <= *cursor) return DATA_LEN_TOO_SMALL;
  unsigned short endpoint_cursor = 0;
  if(!endpoint) {
    for(; *cursor < len && data[*cursor] != ' '; (*cursor)++, endpoint_cursor++);
  } else {
    for(; *cursor < len && endpoint_cursor < *max_len && data[*cursor] != ' '; (*cursor)++, endpoint_cursor++) {
      endpoint[endpoint_cursor] = data[*cursor];
    }
  }
  if(len <= *cursor) return DATA_LEN_TOO_SMALL;
  *max_len = endpoint_cursor;

  return SUCCESS;
}
/*
'parse_parse_endpoint' must be called first. 
If 'version' is NULL, will return cursor pointed to first header section byte.
*/
enum HTTP_PARSE_CODE METHOD_PREFIX parse_http_version(char* data, unsigned short len, unsigned short* cursor, enum HTTP_VERSION* version) {
  if(len <= *cursor || len - *cursor < 8) return DATA_LEN_TOO_SMALL;
  if(data[*cursor] != 'H' || data[(*cursor)+1] != 'T' || data[(*cursor)+2] != 'T' || data[(*cursor)+3] != 'P' || data[(*cursor)+4] != '/' || data[(*cursor)+6] != '.') {
    return INVALID_DATA;
  }
  if(!version) {
    for(;*cursor < len && data[*cursor] >= 'A' && data[*cursor] <= 'Z'; (*cursor)++);
    if(len <= *cursor) return DATA_LEN_TOO_SMALL;
    return SUCCESS;
  }
  switch(data[(*cursor)+5]) {
    case '1': {
      switch(data[(*cursor)+7]) {
        case '0': {
          *version = HTTP_1;
          *cursor += 8;
          return SUCCESS;
        }
        case '1': {
          *version = HTTP_1_1;
          *cursor += 8;
          return SUCCESS;
        }
      }
      return INVALID_DATA;
    }
    case '3': 
    case '2': {
      if(data[(*cursor)+7] != '0') return INVALID_DATA;
      *version = data[(*cursor)+7] == '2' ? HTTP_2 : HTTP_3;
      *cursor += 8;
      return SUCCESS;
    }
  }
  return INVALID_DATA;
}

unsigned short METHOD_PREFIX get_string_size_until(char* data, unsigned short max_len, char delimiter) {
  unsigned short i = 0;
  for(; i < max_len && data[i] != delimiter; i++);
  if(data[i] == delimiter) return i;
  return -1;
}

/*
parse until you find double newline or reach len
returns whether you ended properly in a new line or not
*/
uint8_t METHOD_PREFIX skip_to_request_end(char* data, unsigned short len, unsigned short* cursor) {
  uint8_t just_parsed_newline = 0;
  for(; data[*cursor] && *cursor < len; (*cursor)++) {
    if(just_parsed_newline) {
      switch(data[*cursor]) {
        case '\n':
          (*cursor)++;
          return 1;
        case '\r':
          continue;
        default:
          just_parsed_newline = 0;
          continue;
      }
    }
    just_parsed_newline = (data[*cursor] == '\n');
  }
  return data[*cursor] == '\n';
}


uint8_t METHOD_PREFIX cstrncmp(char* a, char* b, unsigned short len) {
  for(unsigned short i = 0; i < len; i++) {
    if(a[i] != b[i]) return 0;
  }
  return 1;
}

void METHOD_PREFIX parse_key(char* data, unsigned short* cursor, char** headers, unsigned short num_headers, unsigned short max_key_len, unsigned short str_begin, uint8_t* headers_to_go, short* selected_header) {
  unsigned short str_len = MIN(max_key_len, (*cursor) - str_begin);
  uint8_t header_found = 0;
  for(unsigned short i = 0; i < num_headers && !header_found; i++) {
    if(headers_to_go[i]) {
      if(cstrncmp(&data[str_begin], headers[i], str_len)) {
        headers_to_go[i] = 0;
        *selected_header = i;
        header_found = 1;
      }
    }
  }
}

void METHOD_PREFIX parse_value(char* data, unsigned short* cursor, unsigned short str_begin, char* writable, unsigned short max_value_len) {
  unsigned short str_len = MIN(max_value_len, (*cursor) - str_begin);
  for(unsigned short i = str_begin; i < (*cursor); i++) {
    writable[i - str_begin] = data[i];
  }
}

/*
'parse_http_version' must be called first. 
If 'headers' is NULL, will return cursor pointed to first payload byte.
'headers' must be an array of arrays, where each element has a key header as the first item and a writable address of 'max_value_len' len as the second.
*/
enum HTTP_PARSE_CODE METHOD_PREFIX parse_headers(char* data, unsigned short len, unsigned short* cursor, char** headers, unsigned short num_headers, unsigned short max_key_len, unsigned short max_value_len) {
  if(len <= *cursor) return DATA_LEN_TOO_SMALL;
  // loop until you find a double newline or hit len
  if(!headers) {
    skip_to_request_end(data, len, cursor);
    return SUCCESS;
  }
  uint8_t headers_to_go[num_headers];
  short selected_header = -1;
  for(unsigned short i = 0; i < num_headers; i++) headers_to_go[i] = 1;
  uint8_t just_parsed_newline = 0;
  uint8_t carriage_return = 0;
  unsigned short str_begin = *cursor;
  for(; data[*cursor] && *cursor < len; (*cursor)++) {
    switch(data[*cursor]) {
      case '\n': {
        if(just_parsed_newline) return SUCCESS;
        just_parsed_newline = 1;
        if(str_begin && selected_header == -1) {
          // parse value
          parse_value(data, cursor - carriage_return, str_begin, headers[selected_header], max_value_len);
          str_begin = 0;
          carriage_return = 0;
        } else {
          return INVALID_DATA;
        }
      }
      case '\r': {
        if(str_begin && selected_header == -1) {
          carriage_return = 1;
        } else {
          return INVALID_DATA;
        }
        continue;
      }
      case ':': {
        if(!str_begin) return INVALID_DATA;
        // parse key
        parse_key(data, cursor, headers, num_headers, max_key_len, str_begin, headers_to_go, &selected_header);
        str_begin = 0;
      }
    }
  }
  return INVALID_DATA;
}

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
) {
  if(!len) return NULL_LEN;
  unsigned short cursor = 0;
  enum HTTP_PARSE_CODE result;
  if(( result = parse_method(data, *len, &cursor, method) )) goto _return_result;
  os_printf("left after method parsing: '%s'\n", &data[cursor]);
  GO_TO_FIRST_NON_WHITESPACE(data, *len, &cursor);
  os_printf("left after non whitespace parsing: '%s'\n", &data[cursor]);
  if(( result = parse_endpoint(data, *len, &cursor, endpoint, max_endpoint_len) )) goto _return_result;
  os_printf("left after endpoint parsing: '%s'\n", &data[cursor]);
  GO_TO_FIRST_NON_WHITESPACE(data, *len, &cursor);
  os_printf("left after non whitespace parsing: '%s'\n", &data[cursor]);
  if(( result = parse_http_version(data, *len, &cursor, version) )) goto _return_result;
  os_printf("left after version parsing: '%s'\n", &data[cursor]);
  PARSE_NEWLINE(data, len, &cursor);
  os_printf("left after newline parsing: '%s'\n", &data[cursor]);
  if(( result = parse_headers(data, *len, &cursor, headers, num_headers, max_key_len, max_value_len) )) goto _return_result;
  os_printf("len: %d, cursor: %d\n", *len, cursor);
  if(*len == cursor) return SUCCESS;
  if(*len < cursor || *len - cursor < 2) return DATA_LEN_TOO_SMALL;
  if( data[cursor] == '\n' && data[cursor+1] == '\n') {
    *len = cursor + 2;
    return SUCCESS;
  }
  if(*len <= cursor || *len - cursor < 4) return DATA_LEN_TOO_SMALL;
  if( data[cursor] == '\r' || data[cursor+1] == '\n' || data[cursor+2] == '\r' || data[cursor+3] == '\n') {
    *len = cursor + 4;
    return SUCCESS;
  }
  *len = cursor;

_return_result:
  return result;
}
