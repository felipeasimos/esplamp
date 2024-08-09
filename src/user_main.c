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
// (PERIOD * 1000 / 45)
#define MAX_DUTY (PERIOD * 1000 / 45)
#define RGB_TRANSITION_TIMER 50
#define PWM_STEP (RGB_TRANSITION_TIMER * MAX_DUTY * 0.0004)

#define WEBSERVER_PORT 80

static os_timer_t ptimer;
static int32_t rgb_duties[3] = {MAX_DUTY, MAX_DUTY/4, MAX_DUTY/2};

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
  static int8_t directions[3] = {1, 1, 1};

  os_timer_disarm(&ptimer);
  // if(wifi_station_get_connect_status() != STATION_GOT_IP) {
  //   os_printf(".");
  //   wifi_station_disconnect();
  //   wifi_station_connect();
  //   os_timer_setfn(&ptimer, (os_timer_func_t *)rgb_transition, NULL);
  //   os_timer_arm(&ptimer, 1000, 1);
  //   return;
  // }

  for(uint8_t channel = 0; channel < sizeof(rgb_duties)/sizeof(rgb_duties[0]); channel++) {
    rgb_duties[channel] += directions[channel] * PWM_STEP;
    if(rgb_duties[channel] >= MAX_DUTY) {
      directions[channel] = -directions[channel];
      rgb_duties[channel] = MAX_DUTY - 1;
    } else if(rgb_duties[channel] <= 0) {
      directions[channel] = -directions[channel];
      rgb_duties[channel] = 1;
    }
    pwm_set_duty(rgb_duties[channel], channel);
  }
  pwm_start();

  os_timer_setfn(&ptimer, (os_timer_func_t *)rgb_transition, NULL);
  os_timer_arm(&ptimer, RGB_TRANSITION_TIMER, 1);
}

void ICACHE_FLASH_ATTR recv_callback(void* arg, char* data, unsigned short len) {
  char* method = NULL;
  uint8_t method_len = 0;
  char* path = NULL;
  uint32_t path_len = 0;
  mh_version version;
  char* response = "HTTP/1.1 200 OK\r\n\n\r\n";
  char* badresponse = "HTTP/1.1 404 OK\r\n\n\r\n";
  char* data_end = data + len;
  if( (data = mh_parse_request_first_line(data, data_end, &method, &method_len, &path, &path_len, &version)) == NULL ) {
    os_printf("error parsing first line!\n");
    goto error;
  }
  mh_header headers[2] = {0};
  headers[0].header_key_begin = "Content-Length";
  headers[0].header_key_len = os_strlen("Content-Length");
  headers[1].header_key_begin = "Content-Type";
  headers[1].header_key_len = os_strlen("Content-Type");

  if( (data = mh_parse_headers_set(data, data_end, headers, 2)) == NULL) {
    os_printf("error parsing headers!\n");
    goto error;
  }

  struct jsonparse_state state;
  jsonparse_setup(&state, data, data_end - data);
  state.stack[0] = '[';
  state.depth++;
  state.vtype = 0;
  uint8_t rgb_values[3] = {0};
  int type = jsonparse_get_type(&state);
  os_printf("data: %s\n", data);
  if(( type = jsonparse_next(&state)) != JSON_TYPE_ARRAY) {
    os_printf("not array\n");
    goto error;
  }
  if(( type = jsonparse_next(&state)) != JSON_TYPE_NUMBER) {
    os_printf("not int for red\n");
    goto error;
  }
  rgb_values[0] = jsonparse_get_value_as_int(&state);
  if(( type = jsonparse_next(&state)) != ',') {
    os_printf("no comma after red\n");
    goto error;
  }
  if(( type = jsonparse_next(&state)) != JSON_TYPE_NUMBER) {
    os_printf("not int for green\n");
    goto error;
  }
  rgb_values[1] = jsonparse_get_value_as_int(&state);
  if(( type = jsonparse_next(&state)) != ',') {
    os_printf("no comma after green\n");
    goto error;
  }
  if(( type = jsonparse_next(&state)) != JSON_TYPE_NUMBER) {
    os_printf("not int for blue\n");
    goto error;
  }
  rgb_values[2] = jsonparse_get_value_as_int(&state);
  os_printf("r: %u, g: %u, b: %u\n", rgb_values[0], rgb_values[1], rgb_values[2]);


  if(espconn_send(arg, (uint8_t*)response, os_strlen(response))) {
    goto error;
  }
error:
  os_printf("type: %u\n", type);
  if(type == 0) {
    os_printf("json error: %u\n", state.error);
  }
  espconn_send(arg, (uint8_t*)badresponse, os_strlen(badresponse));
done:
  os_printf("method %c\n", *method);
  os_printf("endpoint: '%c'\n", *path);
  os_printf("version %u\n", version);
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
