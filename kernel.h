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
#define SUCCESS 1
#define OK      1

#define DEADLINE_REACHED    0
#define NOT_EMPTY           0

#define SENDER      +1
#define RECEIVER    -1

#define UINT_MAX 0xffffffff

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
 * @brief Task Control Block (TCB).
 */
typedef struct {
    void    (*PC)();                /**< Program Counter. */
    uint    *SP;                    /**< Stack Pointer. */
    uint    Context[CONTEXT_SIZE];  /**< Context placeholder. */
    uint    StackSeg[STACK_SIZE];   /**< Stack segment. */
    uint    DeadLine;
    uint    SPSR;
} TCB;
#endif

/**
 * @brief Message item.
 */
typedef struct msgobj {
    char            *pData;         /**< Pointer to message data. */
    exception       Status;         /**< Message status. */
    struct l_obj    *pBlock;        /**< Pointer to blocking item. */
    struct msgobj   *pPrevious;     /**< Pointer to previous msg in mailbox. */
    struct msgobj   *pNext;         /**< Pointer to next msg in mailbox. */
} msg;

/**
 * @brief Mailbox structure
 */
typedef struct {
    msg             *pHead;         /**< Pointer to the head of the mailbox. */
    msg             *pTail;         /**< Pointer to the tail of the mailbox. */
    int             nDataSize;      /**< Data size in bytes. */
    int             nMaxMessages;   /**< Maximum number of msgs. */
    int             nMessages;      /**< Current number of msgs. */
    int             nBlockedMsg;    /**< Current number of blocking msgs. */
} mailbox;

/////////////////////////
// TASK ADMINISTRATION //
/////////////////////////

/**
 * @brief Intializes the kernel and its data structures.
 *
 * Leaves the kernel in start-up mode. The call to this function must be made
 * before any other call is made to the kernel.
 *
 * @return OK if success, FAIL otherwise.
 */
exception init_kernel(void);

/**
 * @brief Creates a new task.
 *
 * If the call is made in start-up mode, only necessary data structures will be
 * created. Otherwise, it will lead to a rescheduling and possibly a context
 * switch.
 *
 * @param body      Pointer to the function holding the code of the task.
 * @param deadline  Task's deadline that the kernel will try to meet.
 * @return          OK if success, FAIL otherwise.
 */
exception create_task(void (*body)(), uint deadline);

/**
 * @brief Starts the kernel and thus the system of created tasks.
 *
 * Leaves the control to the task with the tightiest deadline. Therefore, it
 * must be placed last in the application initialization code.
 */
void run(void);

/**
 * @brief Terminate the running task.
 *
 * All data structures for the task will be removed. Another task will then
 * be scheduled for execution.
 */
void terminate(void);

///////////////////
// COMMUNICATION //
///////////////////

/**
 * @brief Create a new mailbox.
 *
 * @param  nMessages Maximum number of messages in the mailbox.
 * @param  nDataSize Size of one message in the mailbox, in bytes.
 * @return           Pointer to the created mailbox or NULL.
 */
mailbox* create_mailbox(uint nMessages, uint nDataSize);

/**
 * @brief Remove the mailbox if it is empty.
 *
 * @param  mBox Pointer to the mailbox to be removed.
 * @return      OK if removed, NOT_EMPTY otherwise.
 */
exception remove_mailbox(mailbox* mBox);

/**
 * @brief Return the current number of messages in the specified mailbox.
 * @param  mBox Pointer to the specified mailbox.
 * @return      Number of messages in the specified mailbox.
 */
int no_messages(mailbox* mBox);

/**
 * @brief Send a blocking message to the specified mailbox.
 *
 * @attention Blocking and non blocking messages can't be mixed in a mailbox.
 *
 * @param  mBox  Pointer to the specified mailbox.
 * @param  pData Pointer to the memory area where is situated the msg data.
 * @return       OK if success, DEADLINE_REACHED otherwise.
 */
exception send_wait(mailbox* mBox, void* pData);

/**
 * @brief Blocking attempt to receive a message from the specified mailbox.
 *
 * @param  mBox  Pointer to the specified mailbox.
 * @param  pData Pointer to the memory area where data should be stored.
 * @return       OK if success, DEADLINE_REACHED otherwise.
 */
exception receive_wait(mailbox* mBox, void* pData);

/**
 * @brief Send a non blocking message to the specified mailbox.
 *
 * @attention Blocking and non blocking messages can't be mixed in a mailbox.
 *
 * @param  mBox  Pointer to the specified mailbox.
 * @param  pData Pointer to the memory area where is situated the msg data.
 * @return       OK if success, FAIL otherwise.
 */
exception send_no_wait(mailbox* mBox, void* pData);

/**
 * @brief Non blocking attempt to receive a message from the specified mailbox.
 *
 * @param  mBox  Pointer to the specified mailbox.
 * @param  pData Pointer to the memory area where data should be stored.
 * @return       OK if success, FAIL otherwise.
 */
exception receive_no_wait(mailbox* mBox, void* pData);

////////////
// TIMING //
////////////

/**
 * @brief Block the calling tasks until the given number of ticks has expired.
 *
 * @param  nTicks Number of ticks.
 * @return        OK if success, DEADLINE_REACHED otherwise.
 */
exception wait(uint nTicks);

/**
 * @brief Set the tick counter to the given value.
 * @param no_of_ticks New value for the tick counter.
 */
void set_ticks(uint no_of_ticks);

/**
 * @brief Return the current value of the tick counter.
 *
 * @return  Current 32 bits value of the tick counter.
 */
uint ticks(void);

/**
 * @brief Return the deadline of the current task.
 *
 * @return  Deadline of the current task.
 */
uint deadline(void);

/**
 * @brief Set the deadline of the calling task to the new value.
 *
 * The task will be rescheduled and a context switch might occur.
 *
 * @param nNew New deadline value given in number of ticks.
 */
void set_deadline(uint nNew);

/**
 * @brief Increment the tick counter.
 *
 * @attention This function is called by an ISR and is not available
 *            for the user to call.
 */
void TimerInt(void);

///////////////
// INTERRUPT //
///////////////

extern void isr_off(void);
extern void isr_on(void);
extern void SaveContext(void); // Stores DSP registers in TCB pointed to by Running
extern void LoadContext(void); // Restores DSP registers from TCB pointed to by Running

//////////
// IDLE //
//////////

void IDLE(void) { while(1); };

//////////
// DATA //
//////////

TCB* Running;

list* readyList;
list* timerList;
list* waitingList;

int MODE = -1;

uint TC;

#endif