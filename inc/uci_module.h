/*
 * uci_module.h
 *
 *  Created on: Jan 14, 2023
 *      Author: vanaluk
 */


#ifndef UCI_MODULE_H_
#define UCI_MODULE_H_

#include "global.h"
#include "app_module.h"

ret_t uci_get_value(const char *entry_name, char* out, int out_length);
ret_t uci_set_value(const char *option, const char *value);

#endif /* UCI_MODULE_H_ */
