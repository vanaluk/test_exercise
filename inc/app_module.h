/*
 * global.h
 *
 *  Created on: Jan 14, 2023
 *      Author: vanaluk
 */

#ifndef APP_MODULE_H_
#define APP_MODULE_H_

#include "global.h"

/*
 * example /etc/config/wm2022
 *
 * config wimark_config 'wimark_test_exercise'
 *     option period '60'
 *     option server 'my.mqtt.com'
 *     option topic '/my/secret/topic'
 *     option enabled '1'
 *
*/

#define UCI_CONFIG_FILE    "wm2022"
#define UCI_CONFIG_SECTION "wimark_test_exercise"

#define UCI_PROP_PERIOD    "period"
#define UCI_PROP_SERVER    "server"
#define UCI_PROP_TOPIC     "topic"
#define UCI_PROP_ENABLED   "enabled"

typedef struct app_context_s
{
  char uci_config_path[MAX_PATH];
  char server[MAX_PATH];
  char topic[MAX_PATH];
  int  period;
  bool enabled;
  bool app_thread_running;
  bool ubus_thread_running;
  int  last_period;
  pthread_mutex_t lock;
} app_context_t;

extern app_context_t app_ctx;

ret_t app_init(char *uci_config_file, char *uci_config_section);
void app_deinit(void);

void  app_global_lock(void);
void  app_global_unlock(void);

ret_t app_set_period(int period);
int app_get_period(void);

ret_t app_main(void* arg);

bool app_is_running(void);
void app_stop(void);

#endif /* APP_MODULE_H_ */
