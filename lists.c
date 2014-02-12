/**
 * @file lists.c
 * @brief List management system implementation.
 */

exception insert_readyList(listobj* elmt) {
    listobj* current = readyList->pHead;
    if(readyList->pHead == NULL) { // If empty list
        readyList->pHead = elmt;
        readyList->pTail = elmt;
        return OK;
    }
    while(current->pNext != NULL) { // Until we reach the end of the list
        if(elmt->pTask->DeadLine < current->pTask->DeadLine) {
            current->pPrevious->pNext = elmt;
            elmt->pPrevious = current->pPrevious;
            current->pPrevious = elmt;
            elmt.pNext = current;
            return OK;
        }
        current = current->pNext;
    }
    current->pNext = elmt;
    elmt->pPrevious = current;
    return OK;
}

listobj* extract_readyList(void) {
    listobj* first = readyList->pHead;
    if(first->pNext == NULL) {  // If only one element in list
        readyList->pHead = NULL;
        readyList->pTail = NULL;
        return first;
    }
    first->pNext->pPrevious = NULL;
    readyList->pHead = first->pNext;
    first->pNext = NULL;
    return first;
}

exception insert_timerList(listobj* elmt, uint nTCnt) {
    listobj* current = timerList->pHead;
    elmt->nTCnt = nTCnt;    // Affect value of nTCnt to elmt placeholder
    if(timerList->pHead == NULL) { // If empty list
        timerList->pHead = elmt;
        timerList->pTail = elmt;
        return OK;
    }
    while(current->pNext != NULL) { // Until we reach the end of the list
        if(elmt->nTCnt < current->nTCnt) {
            current->pPrevious->pNext = elmt;
            elmt->pPrevious = current->pPrevious;
            current->pPrevious = elmt;
            elmt.pNext = current;
            return OK;
        }
        current = current->pNext;
    }
    current->pNext = elmt;
    elmt->pPrevious = current;
    return OK;
}

listobj* extract_timerList(void) {
    listobj* first = timerList->pHead;
    if(first->pNext == NULL) {  // If only one element in list
        timerList->pHead = NULL;
        timerList->pTail = NULL;
    }
    else {
        first->pNext->pPrevious = NULL;
        timerList->pHead = first->pNext;
        firs->pNext = NULL;    
    }
    first->nTCnt = 0;
    return first;
}

exception insert_waitingList(listobj* elmt) {
    listobj* current = waitingList->pHead;
    if(waitingList->pHead == NULL) { // If empty list
        waitingList->pHead = elmt;
        waitingList->pTail = elmt;
        return OK;
    }
    while(current->pNext != NULL) { // Until we reach the end of the list
        if(elmt->pTask->DeadLine < current->pTask->DeadLine) {
            current->pPrevious->pNext = elmt;
            elmt->pPrevious = current->pPrevious;
            current->pPrevious = elmt;
            elmt.pNext = current;
            return OK;
        }
        current = current->pNext;
    }
    current->pNext = elmt;
    elmt->pPrevious = current;
    return OK;
}