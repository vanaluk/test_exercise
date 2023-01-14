/*
 * errors.h
 *
 *  Created on: Jan 14, 2023
 *      Author: vanaluk
 */

#ifndef APP_ERRORS_H_
#define APP_ERRORS_H_

#include <stdio.h>
#include <stdlib.h>

#define RET_OK              0U
#define RET_ERROR           1U
#define RET_BLOCKED         2U
#define RET_EXIT            3U
#define RET_BUSY            4U
#define RET_EOF             5U
#define RET_OUT_OF_RANGE    6U
#define RET_EMPTY           7U
#define RET_CANCELLED       8U

#define UNUSED(v)      (void)v

typedef int ret_t;

#endif /* APP_ERRORS_H_ */
