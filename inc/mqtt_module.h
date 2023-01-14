/*
 * mqtt_module.h
 *
 *  Created on: Jan 14, 2023
 *      Author: vanaluk
 */

#ifndef MQTT_MODULE_H_
#define MQTT_MODULE_H_

#include "global.h"
#include "app_module.h"

void* mqtt_module_main(void *arg);
ret_t mqtt_client_reinit(void);

#endif /* MQTT_MODULE_H_ */
