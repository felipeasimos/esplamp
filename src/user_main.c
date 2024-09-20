#include "c_types.h"
#include "eagle_soc.h"
#include "ip_addr.h"
#include "json.h"
#include "osapi.h"
#include "user_interface.h"
#include "pwm.h"
#include "gpio.h"
#include "espconn.h"
#include <minhttp.h>
#include "jsontree.h"
#include "jsonparse.h"

#ifndef WIFI_SSID
#error "Please specify WIFI_SSID"
#endif
#ifndef WIFI_PASSWORD
#error "Please specify WIFI_PASSWORD"
#endif

#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)

#define PERIOD 10000
#define MAX_DUTY (PERIOD * 1000 / 45)
#define RGB_TRANSITION_TIMER 50
#define PWM_STEP ((RGB_TRANSITION_TIMER * MAX_DUTY)/5000)
#define STRLEN(s) ((sizeof(s)/sizeof(s[0])) - 1)

#define WEBSERVER_PORT 80

static const char content_length[] = "Content-Length";

static os_timer_t ptimer;
static int32_t rgb_duties[3] = {MAX_DUTY, MAX_DUTY/4, MAX_DUTY/2};
static uint32_t rgb_values[3] = {0};
static int8_t directions[3] = {1, 1, 1};
static uint32_t pwm_step = PWM_STEP;

/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABBBCDDD
 *                A : rf cal
 *                B : at parameters
 *                C : rf init data
 *                D : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        case FLASH_SIZE_64M_MAP_1024_1024:
            rf_cal_sec = 2048 - 5;
            break;
        case FLASH_SIZE_128M_MAP_1024_1024:
            rf_cal_sec = 4096 - 5;
            break;
        default:
            rf_cal_sec = 0;
            break;
    }
    return rf_cal_sec;
}
void ICACHE_FLASH_ATTR user_spi_flash_dio_to_qio_pre_init(void) {}


void ICACHE_FLASH_ATTR rgb_transition(void *arg)
{

  os_timer_disarm(&ptimer);

  for(uint8_t channel = 0; channel < 3; channel++) {
    if(!directions[channel]) continue;
    uint32_t new_rgb_value = rgb_duties[channel] + directions[channel] * pwm_step;
    if(directions[channel] == -1 && new_rgb_value > rgb_duties[channel]) {
      rgb_duties[channel] = rgb_values[channel];
      continue;
    }
    rgb_duties[channel] = new_rgb_value;
    if(directions[channel] == 1) {
      if(rgb_duties[channel] >= rgb_values[channel]) {
        rgb_duties[channel] = rgb_values[channel];
      }
    } else if(directions[channel] == -1) {
      if(rgb_duties[channel] <= rgb_values[channel]) {
        rgb_duties[channel] = rgb_values[channel];
      }
    }
    pwm_set_duty(rgb_duties[channel], channel);
  }
  pwm_start();

  // os_printf("r: %u, g: %u, b: %u\n", rgb_duties[0], rgb_duties[1], rgb_duties[2]);

  os_timer_setfn(&ptimer, (os_timer_func_t *)rgb_transition, NULL);
  os_timer_arm(&ptimer, RGB_TRANSITION_TIMER, 0);
}

uint8_t str_to_int(char* str, uint8_t len) {
  uint8_t sum = 0;
  for(uint8_t i = 0; i < len; i++) {
    sum = (sum * 10) + (str[i] - '0');
  }
  return sum;
}

void int_to_str_with_len_3(char* str, uint8_t n) {
  os_printf("int to str: %d\n", n);
  if(n >= 100) {
    str[0] = '1';
    str[1] = '0';
    str[2] = '0';
    return;
  }
  str[0] = '0';
  str[1] = (n / 10) + '0';
  str[2] = (n % 10) + '0';
  return;
}

uint8_t ICACHE_FLASH_ATTR handle_get(void* espconn) {
  char* okresponse = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\n000000000000";
  int_to_str_with_len_3(&okresponse[39], (pwm_step * 100) / PWM_STEP);
  int_to_str_with_len_3(&okresponse[42], (rgb_values[0] * 100) / MAX_DUTY);
  int_to_str_with_len_3(&okresponse[45], (rgb_values[1] * 100) / MAX_DUTY);
  int_to_str_with_len_3(&okresponse[48], (rgb_values[2] * 100) / MAX_DUTY);
  if(espconn_send(espconn, (uint8_t*)okresponse, 51)) {
    return 0;
  }
  return 1;
}

uint8_t ICACHE_FLASH_ATTR handle_post(void* espconn, char* data, unsigned short len) {
  if(len != 12) return 0;
  pwm_step = (str_to_int(data, 3) * PWM_STEP) / 100;
  for(uint8_t i = 0; i < 3; i++) {
      uint32_t new_rgb_value = str_to_int(&data[(i * 3) + 3], 3) * MAX_DUTY / 100;
      directions[i] = new_rgb_value > rgb_values[i] ? 1 : -1;
      rgb_values[i] = new_rgb_value;
  }

  os_printf("r: %u, g: %u, b: %u\n", rgb_duties[0], rgb_duties[1], rgb_duties[2]);

  return handle_get(espconn);
}


void ICACHE_FLASH_ATTR recv_callback(void* arg, char* data, unsigned short len) {

  os_printf("recv_callback\n");
  os_timer_disarm(&ptimer);
  char* method = NULL;
  uint8_t method_len = 0;
  char* badresponse = "HTTP/1.1 404 Bad Request\r\nContent-Length: 0\r\n\r\n";
  char* data_end = data + len;
  if( (data = mh_parse_request_first_line(data, data_end, &method, &method_len, NULL, NULL, NULL)) == NULL ) {
    os_printf("error parsing first line!\n");
    goto error;
  }
  mh_header header = {
    .header_key_begin = (char*)content_length,
    .header_key_len = STRLEN(content_length),
    .header_value_len = 0,
    .header_value_begin = 0
  };
  if( (data = mh_parse_headers_set(data, data_end, &header, 1)) == NULL) {
    os_printf("error parsing headers!\n");
    goto error;
  }
  uint8_t payload_size = str_to_int(header.header_value_begin, header.header_value_len);

  len = data_end - data;
  switch(*method) {
    case 'G': {
      uint8_t res = handle_get(arg);
      if(!res) goto error;
      break;
    }
    case 'P': {
      if(payload_size != 12) goto error;
      uint8_t res = handle_post(arg, data, len);
      if(!res) goto error;
      break;
    }
    default: {
      goto error;
    }
  }
  goto done;
error:
  espconn_send(arg, (uint8_t*)badresponse, os_strlen(badresponse));
done:
  os_timer_setfn(&ptimer, (os_timer_func_t *)rgb_transition, NULL);
  os_timer_arm(&ptimer, RGB_TRANSITION_TIMER, 0);
}

void ICACHE_FLASH_ATTR sent_callback(void* arg) {
  os_printf("sent cb\n");
}

void ICACHE_FLASH_ATTR disconnect_callback(void* arg) {
  os_printf("disconnect cb\n");
}

void ICACHE_FLASH_ATTR reconnect_callback(void* arg, int8_t err) {
  os_printf("reconnect cb\n");
}

void ICACHE_FLASH_ATTR connect_callback(void* arg) {
  os_printf("connect cb\n");
  espconn_regist_recvcb(arg, recv_callback);
  espconn_regist_reconcb(arg, reconnect_callback);
  espconn_regist_disconcb(arg, disconnect_callback);
}

void ICACHE_FLASH_ATTR block_for_wifi(void) {
    static esp_tcp esptcp = {
      .local_port = WEBSERVER_PORT
    };
    static struct espconn espconn = {
      .type = ESPCONN_TCP,
      .state = ESPCONN_NONE,
      .proto.tcp = &esptcp,
    };
    os_timer_disarm(&ptimer);
    if(wifi_station_get_connect_status() != STATION_GOT_IP) {
      os_timer_setfn(&ptimer, (os_timer_func_t *)block_for_wifi, NULL);
      os_timer_arm(&ptimer, 1000, 1);
      return;
    }
    espconn_regist_connectcb(&espconn, connect_callback);

    os_printf("espconn_accept response: %u\n", espconn_accept(&espconn));
    os_timer_setfn(&ptimer, (os_timer_func_t *)rgb_transition, NULL);
    os_timer_arm(&ptimer, RGB_TRANSITION_TIMER, 1);
}

void ICACHE_FLASH_ATTR user_init(void)
{
    // set serial communication
    uart_init(9600, 9600);
    os_printf("SDK version:%s\n", system_get_sdk_version());

    // set pwm settings
    uint32 io_info[3][3] = {
      {PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5, 5},
      {PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4, 4},
      {PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14, 14}
    };
    pwm_init(PERIOD, (uint32_t*)rgb_duties, sizeof(rgb_duties)/sizeof(rgb_duties[0]), io_info);

    // set wifi settings
    if(!wifi_set_opmode_current(STATION_MODE)) {
      os_printf("wifi_set_opmode failed\n");
      system_restart();
    }

    struct station_config station_cfg = {
      .ssid = STR(WIFI_SSID),
      .bssid = {0},
      .bssid_set = 0,
      .password = STR(WIFI_PASSWORD)
    }; 


    wifi_station_set_config(&station_cfg);
    wifi_station_set_reconnect_policy(true);
    wifi_station_set_auto_connect(true);

    system_init_done_cb(block_for_wifi);
}
