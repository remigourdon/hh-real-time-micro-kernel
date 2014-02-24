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

#ifdef texas_dsp

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

#ifdef texas_dsp
typedef struct {
    void    (*PC)();
    uint    *SP;
    uint    Context[CONTEXT_SIZE];
    uint    StackSeg[STACK_SIZE];
    uint    DeadLine;
} TCB;
#else
/**
 * @brief Task Control Block (TCB)
 */
typedef struct {
    void    (*PC)();                /**< Program Counter */
    uint    *SP;                    /**< Stack Pointer */
    uint    Context[CONTEXT_SIZE];  /**< Context placeholder */
    uint    StackSeg[STACK_SIZE];   /**< Stack segment */
    uint    DeadLine;
    uint    SPSR;
} TCB;
#endif

/**
 * @brief Message item
 */
typedef struct msgobj {
    char            *pData;         /**< Pointer to message data */
    exception       Status;         /**< Message status */
    struct l_obj    *pBlock;        /**< Pointer to blocking item */
    struct msgobj   *pPrevious;     /**< Pointer to previous msg in mailbox */
    struct msgobj   *pNext;         /**< Pointer to next msg in mailbox */
} msg;

/**
 * @brief Mailbox structure
 */
typedef struct {
    msg             *pHead;         /**< Pointer to the head of the mailbox */
    msg             *pTail;         /**< Pointer to the tail of the mailbox */
    int             nDataSize;      /**< Data size in bytes */
    int             nMaxMessages;   /**< Maximum number of msgs */
    int             nMessages;      /**< Current number of msgs */
    int             nBlockedMsg;    /**< Current number of blocking msgs */
} mailbox;

#endif