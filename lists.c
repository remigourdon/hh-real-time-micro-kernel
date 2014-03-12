/**
 * @file lists.c
 * @brief List management system implementation.
 */

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

    ist_off();

    if(readyList->pHead->pNext == readyList->pTail) { // If empty list
        readyList->pHead->pNext     = elmt;
        readyList->pTail->pPrevious = elmt;
        elmt->pPrevious             = readyList->pHead;
        elmt->pNext                 = readyList->pTail;

        // Update Running pointer
        Running = readyList->pHead->pNext->TCB;

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
    Running = readyList->pHead->pNext->TCB;

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
    Running = readyList->pHead->pNext->TCB;

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