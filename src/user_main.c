#include "c_types.h"
#include "eagle_soc.h"
#include "ip_addr.h"
#include "osapi.h"
#include "user_interface.h"
#include "pwm.h"
#include "gpio.h"
#include "espconn.h"

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
  if(wifi_station_get_connect_status() != STATION_GOT_IP) {
    os_printf(".");
    wifi_station_disconnect();
    wifi_station_connect();
    os_timer_setfn(&ptimer, (os_timer_func_t *)rgb_transition, NULL);
    os_timer_arm(&ptimer, 1000, 1);
    return;
  }

  for(uint8_t channel = 0; channel < 3; channel++) {
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

  // os_printf("#%u %u %u\n", rgb_duties[0], rgb_duties[1], rgb_duties[2]);
  os_timer_setfn(&ptimer, (os_timer_func_t *)rgb_transition, NULL);
  os_timer_arm(&ptimer, RGB_TRANSITION_TIMER, 1);
}

void ICACHE_FLASH_ATTR block_for_wifi(void) {

    os_timer_disarm(&ptimer);
    if(wifi_station_get_connect_status() != STATION_GOT_IP) {
      os_timer_setfn(&ptimer, (os_timer_func_t *)block_for_wifi, NULL);
      os_timer_arm(&ptimer, 1000, 1);
      return;
    }
    // struct espconn {
    //     /** type of the espconn (TCP, UDP) */
    //     enum espconn_type type;
    //     /** current state of the espconn */
    //     enum espconn_state state;
    //     union {
    //         esp_tcp *tcp;
    //         esp_udp *udp;
    //     } proto;
    //     /** A callback function that is informed about events for this espconn */
    //     espconn_recv_callback recv_callback;
    // 	espconn_sent_callback sent_callback;
    // 	uint8 link_cnt;
    // 	void *reverse;
    // };

    os_timer_setfn(&ptimer, (os_timer_func_t *)rgb_transition, NULL);
    os_timer_arm(&ptimer, RGB_TRANSITION_TIMER, 1);
}

void ICACHE_FLASH_ATTR user_main() {

    // block until wifi is connected
    os_timer_disarm(&ptimer);
    os_timer_setfn(&ptimer, (os_timer_func_t *)block_for_wifi, NULL);
    os_timer_arm(&ptimer, 1000, 1);
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
