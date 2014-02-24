/**
 * @file kernel.h
 * @brief Kernel definitions
 */

#ifndef KERNEL_H
#define KERNEL_H value

//////////////////////////////////////
// GLOBAL VARIABLES AND DEFINITIONS //
//////////////////////////////////////

#include <stdlib.h>
#include "lists.h"

#ifndef texas_dsp

#define CONTEXT_SIZE    34-2

#else

#define CONTEXT_SIZE    13
#define STACK_SIZE      100

#endif

#define TRUE    1
#define FALSE   !TRUE

#define RUNNING 1
#define INIT    !RUNNING

#define FAIL    0
#define SUCCESS !FAIL
#define OK      !FAIL

#define DEADLINE_REACHED    0
#define NOT_EMPTY           0

#define SENDER      +1
#define RECEIVER    -SENDER

typedef int             exception;
typedef int             bool;
typedef unsigned int    uint;
typedef int             action;

#endif