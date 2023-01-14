/*
 * uci_module.h
 *
 *  Created on: Jan 14, 2023
 *      Author: vanaluk
 */


#ifndef UCI_MODULE_H_
#define UCI_MODULE_H_

#include "global.h"

ret_t uci_show_value(const char *uci_path, const char *entry_name);
ret_t uci_set_value(const char *uci_path, const char *option, const char *value);

#endif /* UCI_MODULE_H_ */
