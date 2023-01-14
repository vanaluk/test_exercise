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

#define RET_OK              (0)
#define RET_ERROR           (-1)
#define RET_BLOCKED         (-2)
#define RET_EXIT            (-3)
#define RET_BUSY            (-4)
#define RET_EOF             (-5)
#define RET_OUT_OF_RANGE    (-6)
#define RET_EMPTY           (-7)
#define RET_CANCELLED       (-8)

#define UNUSED(v)      (void)v

typedef int ret_t;

#endif /* APP_ERRORS_H_ */
