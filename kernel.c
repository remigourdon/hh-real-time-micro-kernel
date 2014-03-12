/**
 * @file kernel.c
 * @brief Kernel declarations
 */

#include "kernel.h"

////////////////////
// LISTS MANAGING //
////////////////////

list* create_emptyList(void) {
    list* newList;

    // BEGIN CRITICAL ZONE
    isr_off();
    newList = (list*)malloc(sizeof(list));

    newList->pHead = (listobj*)malloc(sizeof(listobj));
    newList->pTail = (listobj*)malloc(sizeof(listobj));
    isr_on();
    // END CRITICAL ZONE

    newList->pHead->pPrevious = NULL;
    newList->pHead->pNext     = newList->pTail;

    newList->pTail->pNext     = NULL;
    newList->pTail->pPrevious = newList->pHead;

    return newList;
}

exception insert_readyList(listobj* elmt) {
    listobj* current;

    current = readyList->pHead;

    isr_off();

    if(readyList->pHead->pNext == readyList->pTail) { // If empty list
        readyList->pHead->pNext     = elmt;
        readyList->pTail->pPrevious = elmt;
        elmt->pPrevious             = readyList->pHead;
        elmt->pNext                 = readyList->pTail;

        // Update Running pointer
        Running = readyList->pHead->pNext->pTask;

        isr_on();
        return OK;
    }

    while((current->pTask->DeadLine < elmt->pTask->DeadLine) && (current != readyList->pTail)) {
        current = current->pNext;
    }

    elmt->pPrevious = current->pPrevious;
    elmt->pNext = current;
    current->pPrevious->pNext = elmt;
    current->pPrevious = elmt;

    // Update Running pointer
    Running = readyList->pHead->pNext->pTask;

    isr_on();
    return OK;
}

listobj* extract_readyList(void) {
    listobj* elmt;

    isr_off();

    if(readyList->pHead->pNext == readyList->pTail) { // If empty list
        isr_on();
        return NULL;
    }

    elmt = readyList->pHead->pNext;

    readyList->pHead->pNext = elmt->pNext;
    elmt->pNext->pPrevious = readyList->pHead;
    elmt->pPrevious = NULL;
    elmt->pNext = NULL;

    // Update Running pointer
    Running = readyList->pHead->pNext->pTask;

    isr_on();
    return elmt;
}

exception insert_timerList(listobj* elmt, uint nTCnt) {
    listobj* current = timerList->pHead;

    isr_off();

    elmt->nTCnt = ticks() + nTCnt; /// @todo Affect "nTCnt" directly or "ticks() + nTCnt"?

    if(timerList->pHead->pNext == timerList->pTail) { // If empty list
        timerList->pHead->pNext     = elmt;
        timerList->pTail->pPrevious = elmt;
        elmt->pPrevious             = timerList->pHead;
        elmt->pNext                 = timerList->pTail;

        isr_on();
        return OK;
    }

    while((current->nTCnt < elmt->nTCnt) && (current != timerList->pTail)) {
        current = current->pNext;
    }

    elmt->pPrevious = current->pPrevious;
    elmt->pNext = current;
    current->pPrevious->pNext = elmt;
    current->pPrevious = elmt;

    isr_on();
    return OK;
}

listobj* extract_timerList(void) {
    listobj* elmt;

    isr_off();

    if(timerList->pHead->pNext == timerList->pTail) { // If empty list
        isr_on();
        return NULL;
    }

    elmt = timerList->pHead->pNext;
    elmt->nTCnt = 0;

    timerList->pHead->pNext = elmt->pNext;
    elmt->pNext->pPrevious = timerList->pHead;
    elmt->pPrevious = NULL;
    elmt->pNext = NULL;

    isr_on();
    return elmt;
}

exception insert_waitingList(listobj* elmt) {
    listobj* current = waitingList->pHead;

    isr_off();

    if(waitingList->pHead->pNext == waitingList->pTail) { // If empty list
        waitingList->pHead->pNext     = elmt;
        waitingList->pTail->pPrevious = elmt;
        elmt->pPrevious             = waitingList->pHead;
        elmt->pNext                 = waitingList->pTail;

        isr_on();
        return OK;
    }

    while((current->pTask->DeadLine < elmt->pTask->DeadLine) && (current != waitingList->pTail)) {
        current = current->pNext;
    }

    elmt->pPrevious = current->pPrevious;
    elmt->pNext = current;
    current->pPrevious->pNext = elmt;
    current->pPrevious = elmt;

    isr_on();
    return OK;
}

listobj* extract_waitingList(listobj* elmt) {
    isr_off();

    if(elmt != NULL) {
        elmt->pPrevious->pNext = elmt->pNext;
        elmt->pNext->pPrevious = elmt->pPrevious;
        elmt->pPrevious = NULL;
        elmt->pNext = NULL;
    }

    isr_on();
    return elmt;
}

/////////////////////////
// TASK ADMINISTRATION //
/////////////////////////

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

///////////////////
// COMMUNICATION //
///////////////////

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
    else {
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
    else {
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

////////////
// TIMING //
////////////

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
        if(ticks() >= Running->DeadLine) { /// @todo Verify that
            return DEADLINE_REACHED;
        }
        else {
            return OK;
        }
    }
    return OK;
}

void set_ticks(uint no_of_ticks) {
    // Modify the TC value only if the new value is superior to the current one
    if(no_of_ticks > ticks()) {
        TC = no_of_ticks;
    }
}

uint ticks(void) {
    return TC;
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

void TimerInt(void) {
    // Temporary listobj placeholder
    listobj* elmt;

    TC++;

    // Check the timerList for tasks that are ready for execution
    // Move these to readyList
    // Loop until first elmt ticks value superior to current TC value or list is empty
    while(ticks() >= timerList->pHead->pNext->nTCnt || timerList->pHead->pNext == timerList->pTail) {
        elmt = extract_timerList();
        insert_readyList(elmt);
    }

    // Check the waitingList for tasks that have expired deadlines
    // Move these to readyList and clean up their Mailbox entry
    // Loop until first elmt deadline superior to current TC value or list is empty
    while(ticks() >= waitingList->pHead->pNext->pTask->DeadLine || waitingList->pHead->pNext == waitingList->pTail) {
        elmt = extract_waitingList(waitingList->pHead->pNext);
        insert_readyList(elmt);
        // Clean up their Mailbox entry

    }
}