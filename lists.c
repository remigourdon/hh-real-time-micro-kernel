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