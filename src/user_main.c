#include "c_types.h"
#include "eagle_soc.h"
#include "ip_addr.h"
#include "json.h"
#include "osapi.h"
#include "user_interface.h"
#include "pwm.h"
#include "gpio.h"
#include "espconn.h"
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
#define MIN(a, b) (a < b ? a : b)

#define PERIOD 10000
#define MAX_DUTY (PERIOD * 1000 / 45)
#define RGB_TRANSITION_TIMER 50
#define UDP_BROADCAST_TIMER 1000
#define PWM_STEP ((RGB_TRANSITION_TIMER * MAX_DUTY)/5000)
#define STRLEN(s) ((sizeof(s)/sizeof(s[0])) - 1)

#define WEBSERVER_PORT 80
#define DEVICE_DISCOVERY_PORT 12345

static const char discovery_request[] = "whatstheesplampipagain?";
static const char discovery_response[] = "openupitsme";

static os_timer_t ptimer;
static os_timer_t udp_timer;
static int32_t rgb_duties[3] = {MAX_DUTY, MAX_DUTY/4, MAX_DUTY/2};
static uint32_t rgb_values[3] = {0};
static int8_t directions[3] = {1, 1, 1};
static uint32_t pwm_step = PWM_STEP;

static esp_tcp esptcp = {
  .local_port = WEBSERVER_PORT
};
static struct espconn tcp_espconn = {
  .type = ESPCONN_TCP,
  .state = ESPCONN_NONE,
  .proto.tcp = &esptcp,
};

static esp_udp espudp = {
  .local_port = DEVICE_DISCOVERY_PORT,
  .remote_ip = {255, 255, 255, 255},
  .remote_port = 12345
};

static struct espconn udp_espconn = {
  .type = ESPCONN_UDP,
  .state = ESPCONN_NONE,
  .proto.udp = &espudp,
};

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

  // os_timer_setfn(&ptimer, (os_timer_func_t *)rgb_transition, NULL);
  // os_timer_arm(&ptimer, RGB_TRANSITION_TIMER, 0);
}

void ICACHE_FLASH_ATTR generate_response_data(char* data) {
  data[0] = (pwm_step * 100) / PWM_STEP;
  data[1] = (rgb_values[0] * 100) / MAX_DUTY;
  data[2] = (rgb_values[1] * 100) / MAX_DUTY;
  data[3] = (rgb_values[2] * 100) / MAX_DUTY;
}

uint8_t ICACHE_FLASH_ATTR handle_get(void* espconn) {
  char okresponse[] = "\x06\x01\x00\x00\x00\x00";
  generate_response_data(okresponse+2);
  if(espconn_send(espconn, (uint8_t*)okresponse, 6)) {
    return 0;
  }
  return 1;
}

uint8_t ICACHE_FLASH_ATTR handle_post(void* espconn, char* data, unsigned short len) {
  if(len != 4) return 0;
  pwm_step = (data[0] * PWM_STEP) / 100;
  for(uint8_t i = 0; i < 3; i++) {
      uint32_t new_rgb_value = (data[1 + i] * MAX_DUTY) / 100;
      os_printf("data: %d\n", data[1+i]);
      os_printf("MAX_DUTY: %d\n", MAX_DUTY);
      os_printf("new_rgb_value: %d\n", new_rgb_value);
      directions[i] = new_rgb_value > rgb_values[i] ? 1 : -1;
      rgb_values[i] = new_rgb_value;
  }

  os_printf("r: %u, g: %u, b: %u\n", rgb_values[0], rgb_values[1], rgb_values[2]);

  return handle_get(espconn);
}

int strequal(char* a, char* b, uint8_t n) {
  for(uint8_t i = 0; i < n; i++) {
    if(a[i] != b[i] || !a[i] || !b[i]) return 0;
  }
  return 1;
}


void ICACHE_FLASH_ATTR tcp_recv_callback(void* arg, char* data, unsigned short len) {

  os_printf("tcp_recv_callback\n");
  os_timer_disarm(&ptimer);
  if(len != 6) {
    os_printf("invalid length (not 6)\n");
    goto error;
  }
  len = *data;
  data++;

  switch(*data) {
    case 'G': {
      uint8_t res = handle_get(arg);
      if(!res) goto error;
      break;
    }
    case 'P': {
      uint8_t res = handle_post(arg, ++data, len-2);
      if(!res) goto error;
      break;
    }
    default: {
      goto error;
    }
  }
  goto done;
error:
  os_printf("error\n");
  char badresponse[] = "\x06\x00\x00\x00\x00\x00";
  generate_response_data(badresponse+2);
  espconn_send(arg, (uint8_t*)badresponse, 6);
done:
  os_timer_setfn(&ptimer, (os_timer_func_t *)rgb_transition, NULL);
  os_timer_arm(&ptimer, RGB_TRANSITION_TIMER, 0);
}

void ICACHE_FLASH_ATTR sent_callback(void* arg) {
  os_printf("sent cb\n");
}

void ICACHE_FLASH_ATTR tcp_disconnect_callback(void* arg) {
  os_printf("disconnect cb\n");
}

void ICACHE_FLASH_ATTR tcp_reconnect_callback(void* arg, int8_t err) {
  os_printf("reconnect cb\n");
}

void ICACHE_FLASH_ATTR tcp_connect_callback(void* arg) {
  os_printf("connect cb\n");
}

void ICACHE_FLASH_ATTR udp_broadcast(void *arg) {
  if(espconn_sendto(&udp_espconn, (uint8_t*)discovery_response, STRLEN(discovery_response))) {
    os_printf("failed to send udp broadcast beacon\n");
  }
}

void ICACHE_FLASH_ATTR udp_recv_callback(void* espconn, char* data, unsigned short len) {
  os_printf("udp_recv cb\n");
  if( strequal(data, (char*)discovery_request, MIN(len, STRLEN(discovery_request))) ) {
    uint8_t* remote_ip = ((struct espconn*)espconn)->proto.udp->remote_ip;
    remote_ip[0] = 255;
    remote_ip[1] = 255;
    remote_ip[2] = 255;
    remote_ip[3] = 255;
    ((struct espconn*)espconn)->proto.udp->remote_port = DEVICE_DISCOVERY_PORT;
    if(espconn_sendto(espconn, (uint8_t*)discovery_response, STRLEN(discovery_response))) {
      os_printf("failed to send udp response\n");
    }
  }
}

void ICACHE_FLASH_ATTR block_for_wifi(void) {
    os_timer_disarm(&ptimer);
    if(wifi_station_get_connect_status() != STATION_GOT_IP) {
      os_timer_setfn(&ptimer, (os_timer_func_t *)block_for_wifi, NULL);
      os_timer_arm(&ptimer, 1000, 1);
      return;
    }
    os_timer_disarm(&ptimer);
    // tcp
    espconn_regist_connectcb(&tcp_espconn, tcp_connect_callback);
    espconn_regist_recvcb(&tcp_espconn, tcp_recv_callback);
    espconn_regist_reconcb(&tcp_espconn, tcp_reconnect_callback);
    espconn_regist_disconcb(&tcp_espconn, tcp_disconnect_callback);

    // udp
    espconn_regist_recvcb(&udp_espconn, udp_recv_callback);
    uint8_t res = espconn_create(&udp_espconn);
    os_printf("espconn create = %d\n",res);

    os_printf("espconn_accept response: %u\n", espconn_accept(&tcp_espconn));

    os_timer_setfn(&ptimer, (os_timer_func_t *)rgb_transition, NULL);
    os_timer_arm(&ptimer, RGB_TRANSITION_TIMER, 1);

    os_timer_setfn(&udp_timer, (os_timer_func_t*)udp_broadcast, NULL);
    os_timer_arm(&udp_timer, UDP_BROADCAST_TIMER, 1);
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
