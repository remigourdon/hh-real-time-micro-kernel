/**
 * @file lists.c
 * @brief List management system implementation.
 */

exception insert_readyList(listobj* elmt) {
    listobj* current = readyList->pHead;

    if(readyList->pHead == readyList->pTail == NULL) { // If empty list
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
    first->pNext->pPrevious = NULL;
    readyList->pHead = first->pNext;
    firs->pNext = NULL;
    return first;
}

exception insert_timerList(listobj* elmt) {
    listobj* current = timerList->pHead;

    if(timerList->pHead == timerList->pTail == NULL) { // If empty list
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
    first->pNext->pPrevious = NULL;
    timerList->pHead = first->pNext;
    firs->pNext = NULL;
    return first;
}