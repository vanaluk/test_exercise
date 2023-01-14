/*
 * trace.h
 *
 *  Created on: Jan 14, 2023
 *      Author: vanaluk
 */

#ifndef TRACE_H_
#define TRACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

extern bool global_trace_enable;

#define TRACE(a, args...) if (global_trace_enable == true)                                     \
                          {                                                                 \
                            printf("%s(%s:%d) \n" a,  __func__,__FILE__, __LINE__, ##args); \
                            printf("\n");                                                   \
                          }
#define println(a, args...) printf(a "\n", ##args)

#endif /* TRACE_H_ */
