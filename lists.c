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

    if(readyList->pHead->pNext == readyList->pTail) { // If empty list
        readyList->pHead->pNext     = elmt;
        readyList->pTail->pPrevious = elmt;
        elmt->pPrevious             = readyList->pHead;
        elmt->pNext                 = readyList->pTail;

        // Update Running pointer
        Running = readyList->pHead->pNext->TCB;

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

    return OK;
}

listobj* extract_readyList(void) {
    listobj* elmt;

    if(readyList->pHead->pNext == readyList->pTail) { // If empty list
        return NULL;
    }

    elmt = readyList->pHead->pNext;

    readyList->pHead->pNext = elmt->pNext;
    elmt->pNext->pPrevious = readyList->pHead;
    elmt->pPrevious = NULL;
    elmt->pNext = NULL;

    // Update Running pointer
    Running = readyList->pHead->pNext->TCB;

    return elmt;
}

exception insert_timerList(listobj* elmt, uint nTCnt) {
    listobj* current = timerList->pHead;

    elmt->nTCnt = nTCnt;

    if(timerList->pHead->pNext == timerList->pTail) { // If empty list
        timerList->pHead->pNext     = elmt;
        timerList->pTail->pPrevious = elmt;
        elmt->pPrevious             = timerList->pHead;
        elmt->pNext                 = timerList->pTail;
        return OK;
    }

    while((current->nTCnt < elmt->nTCnt) && (current != timerList->pTail)) {
        current = current->pNext;
    }

    elmt->pPrevious = current->pPrevious;
    elmt->pNext = current;
    current->pPrevious->pNext = elmt;
    current->pPrevious = elmt;
    return OK;
}

listobj* extract_timerList(void) {
    listobj* elmt;

    if(timerList->pHead->pNext == timerList->pTail) { // If empty list
        return NULL;
    }

    elmt = timerList->pHead->pNext;
    elmt->nTCnt = 0;

    timerList->pHead->pNext = elmt->pNext;
    elmt->pNext->pPrevious = timerList->pHead;
    elmt->pPrevious = NULL;
    elmt->pNext = NULL;

    return elmt;
}

exception insert_waitingList(listobj* elmt) {
    listobj* current = waitingList->pHead;

    if(waitingList->pHead->pNext == waitingList->pTail) { // If empty list
        waitingList->pHead->pNext     = elmt;
        waitingList->pTail->pPrevious = elmt;
        elmt->pPrevious             = waitingList->pHead;
        elmt->pNext                 = waitingList->pTail;
        return OK;
    }

    while((current->pTask->DeadLine < elmt->pTask->DeadLine) && (current != waitingList->pTail)) {
        current = current->pNext;
    }

    elmt->pPrevious = current->pPrevious;
    elmt->pNext = current;
    current->pPrevious->pNext = elmt;
    current->pPrevious = elmt;
    return OK;
}

listobj* extract_waitingList(listobj* elmt) {
    if(elmt != NULL) {
        elmt->pPrevious->pNext = elmt->pNext;
        elmt->pNext->pPrevious = elmt->pPrevious;
        elmt->pPrevious = NULL;
        elmt->pNext = NULL;
    }
    return elmt;
}