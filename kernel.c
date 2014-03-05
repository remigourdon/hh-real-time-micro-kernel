/**
 * @file kernel.c
 * @brief Kernel declarations
 */

#include "kernel.h"

exception init_kernel(void) {
    // Set tick counter to zero
    set_ticks(0);

    // Create necessary data structures
    readyList = create_emptyList();
    timingList = create_emptyList();
    waitingList = create_emptyList();

    // Set the kernel in startup mode
    MODE = INIT;

    // Create an IDLE task
    create_task(IDLE, UINT_MAX);

    // Return status
    return OK;
}

exception create_task(void (*body)(), uint deadline) {
    // Variable dedicated to avoiding looping after context switching
    // The variable is saved in the stack of the current running task (volatile)
    volatile int first = TRUE;

    listobj* newElmt;

    // BEGIN CRITICAL ZONE
    isr_off();
    newElmt         = (listobj*)malloc(sizeof(listobj));
    newElmt->pTask  = (TCB*)malloc(sizeof(TCB));
    isr_on();
    // END CRITICAL ZONE

    // Initialize listobj attributes
    newElmt->nTCnt      = 0;
    newElmt->pMessage   = NULL;
    newElmt->pPrevious  = NULL;
    newElmt->pNext      = NULL;

    // Set deadline in TCB
    newElmt->pTask->DeadLine = deadline;

    // Set TCB's PC to point to the task body
    newElmt->pTask->PC = body;

    // Set TCB's SP to point to the task segment
    newElmt->pTask->SP = &(newElmt->pTask->StackSeg[STACK_SIZE-1]);

    if(MODE == INIT) {      // If startup mode
        insert_readyList(newElmt);
        return OK;
    }
    else {
        isr_off();
        SaveContext();
        if(first == TRUE) {     // If first execution
            first = FALSE;
            insert_readyList(newElmt);
            LoadContext();
        }
    }
    return OK;
}

void run(void) {
    /// @todo Initialize interrupt timer!!

    // Set the kernel in running mode
    MODE = RUNNING;

    isr_on();
    LoadContext();
}

void terminate(void) {
    listobj* terminatingElmt;

    // Remove running task from readyList
    terminatingElmt = extract_readyList();

    // Destroy structures dynamically allocated
    isr_off();
    free(terminatingElmt->pMessage); /// @todo Check if it is the right way to do it
    free(terminatingElmt->pTask);
    free(terminatingElmt);
    isr_on();

    LoadContext();
}

mailbox* create_mailbox(uint nMessages, uint nDataSize) {
    // Allocate memory for the new mailbox
    mailbox* newMailbox;
    // BEGIN CRITICAL ZONE
    isr_off();
    newMailbox = (*mailbox)malloc(sizeof(mailbox));
    newMailbox->pHead = (msgobj*)malloc(sizeof(msgobj));
    newMailbox->pTail = (msgobj*)malloc(sizeof(msgobj));
    isr_on();
    // END CRITICAL ZONE

    // Initialize mailbox structure
    newMailbox->pHead->pPrevious = NULL;
    newMailbox->pHead->pNext     = newMailbox->pTail;
    newMailbox->pTail->pNext     = NULL;
    newMailbox->pTail->pPrevious = newMailbox->pHead;

    newMailbox->nDataSize        = nDataSize;
    newMailbox->nMaxMessages     = nMessages;
    newMailbox->nMessages        = 0;   // Current nb of messages is zero
    newMailbox->nBlockedMsg      = 0;   // Current nb of blocked msg is zero

    return newMailbox;
}