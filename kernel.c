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
    newMailbox = (mailbox*)malloc(sizeof(mailbox));
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

exception remove_mailbox(mailbox* mBox) {
    if(mBox->nMessages == 0 && mBox->nBlockedMsg == 0) { // If mBox is empty
        // BEGIN CRITICAL ZONE
        isr_off();
        free(mBox->pHead);
        free(mBox->pTail);
        free(mBox);
        isr_on();
        // END CRITICAL ZONE
        return OK;
    }
    else {
        return NOT_EMPTY;
    }
}

int no_messages(mailbox* mBox) {
    return mBox->nMessages + mBox->nBlockedMsg; /// @todo Check if it is correct way
}

exception send_wait(mailbox* mBox, void* pData) {
    // Variable dedicated to avoiding looping after context switching
    // The variable is saved in the stack of the current running task (volatile)
    volatile int first = TRUE;

    // Temporary placeholder for listobj
    listobj* elmt;

    // Message structure placeholder
    msgobj* msg;

    isr_off();
    SaveContext();

    if(first == TRUE) {
        first = FALSE;
        if() {  // If receiving task is waiting

        }
        else {
            // Allocate a message structure
            msg = (msgobj*)malloc(sizeof(msgobj));
            // Copy data to the message
            msg->pData = pData;

            // Add message to the mailbox
            /// @todo Mailbox are FIFO or LIFO???

            // Move sending task from readyList to waitingList
            elmt = extract_readyList();
            insert_waitingList(elmt);
        }
        LoadContext();
    }
    else {  /// @todo How does the sending task end up in the runningList again??
        if(ticks() > Running->DeadLine) {  // If deadline is reached
            // BEGIN CRITICAL ZONE
            isr_off();
            free(msg);
            // Dicreases blocked msg counter
            mBox->nBlockedMsg--;
            isr_on();
            // END CRITICAL ZONE
            return DEADLINE_REACHED;
        }
        else {
            return OK;
        }
    }
}

exception receive_wait(mailbox* mBox, void* pData) {
    // Variable dedicated to avoiding looping after context switching
    // The variable is saved in the stack of the current running task (volatile)
    volatile int first = TRUE;

    // Temporary placeholder for listobj
    listobj* elmt;

    // Message structure placeholder
    msgobj* msg;

    isr_off();
    SaveContext();

    if(first == TRUE) {
        first = FALSE;
        if() {  // If send message is waiting
            if() {  // If send message was of type wait
                // Move sending task to readyList
            }
            else {
                // Free senders data area
            }
        }
        else {
            // Allocate a message structure
            msg = (msgobj*)malloc(sizeof(msgobj));

            // Add message to the mailbox
            /// @todo Mailbox are FIFO or LIFO???

            // Move receiving task from readyList to waitingList
            elmt = extract_readyList();
            insert_waitingList(elmt);
        }
        LoadContext();
    }
    else {  /// @todo How does the receiving task end up in the runningList again??
        if(ticks() > Running->DeadLine) {   // If deadline is reached
            // BEGIN CRITICAL ZONE
            isr_off();
            free(msg);
            // Increases blocked msg counter
            mBox->nBlockedMsg++;
            isr_on();
            // END CRITICAL ZONE
            return DEADLINE_REACHED;
        }
        else {
            return OK;
        }
    }
}

exception send_no_wait(mailbox* mBox, void* pData) {
    // Variable dedicated to avoiding looping after context switching
    // The variable is saved in the stack of the current running task (volatile)
    volatile int first = TRUE;

    // Message structure placeholder
    msgobj* msg;

    isr_off();
    SaveContext();

    if(first == TRUE) {
        first = FALSE;
        if() {  // If receiving task is waiting

        }
        else {
            // Allocate a message structure
            msg = (msgobj*)malloc(sizeof(msgobj));
            // Copy data to the message
            msg->pData = pData;

            if(no_messages(mBox) >= mBox->nMaxMessages) { // If mailbox is full
                // Remove oldest message structure
            }

            // Add message to the mailbox
            /// @todo Mailbox are FIFO or LIFO???
        }
    }
    return OK;
}

exception receive_no_wait(mailbox* mBox, void* pData) {
    // Variable dedicated to avoiding looping after context switching
    // The variable is saved in the stack of the current running task (volatile)
    volatile int first = TRUE;

    // Message structure placeholder
    msgobj* msg;

    isr_off();
    SaveContext();

    if(first == TRUE) {
        first = FALSE;
        if() {  // If send message is waiting
            if() {  // If send message was of type wait
                // Move sending task to readyList
            }
            else {
                // Free senders data area
            }
        }
        LoadContext();
    }

    // Return status on received message
    return OK; /// @todo Check what means the status in the message structure!
}

exception wait(uint nTicks) {
    // Variable dedicated to avoiding looping after context switching
    // The variable is saved in the stack of the current running task (volatile)
    volatile int first = TRUE;

    listobj* elmt;

    isr_off();
    SaveContext();
    if(first == TRUE) {     // If first execution
        first = FALSE;
        elmt = extract_readyList();     // Remove from readyList
        insert_timerList(elmt, nTicks); // Move to timerList
        LoadContext();                  // Load new context
    }
    else {
        if(ticks() > Running->DeadLine) { /// @todo Verify that
            return DEADLINE_REACHED;
        }
        else {
            return OK;
        }
    }
    return OK;
}

uint deadline(void) {
    return Running->DeadLine;
}

void set_deadline(uint nNew) {
    // Variable dedicated to avoiding looping after context switching
    // The variable is saved in the stack of the current running task (volatile)
    volatile int first = TRUE;

    // Temporary listobj handler
    listobj* elmt;

    isr_off();
    SaveContext();
    if(first == TRUE) {
        first = FALSE;
        Running->DeadLine = nNew; /// @todo Verify that

        // Reschedule readyList
        if(nNew > readyList->pHead->pNext->pNext) { // If running deadline not the shortest anymore
            elmt = extract_readyList();
            insert_readyList(elmt);
        }
        LoadContext();
    }
}