/**
 * @file lists.c
 * @brief List management system implementation.
 */

list* create_emptyList(void) {
    list* newList;
    newList = (list*)malloc(sizeof(list));

    newList->pHead = (listobj*)malloc(sizeof(listobj));
    newList->pTail = (listobj*)malloc(sizeof(listobj));

    newList->pHead->pPrevious = NULL;
    newList->pHead->pNext     = newList->pTail;

    newList->pTail->pNext     = NULL;
    newList->pTail->pPrevious = newList->pHead;

    return newList;
}

exception destroy_list(list* l) {
    if(l != NULL) {
        free(l->pHead);
        free(l->pTail);
        free(l);
        return OK;
    }
    return FAIL;
}

exception insert_readyList(listobj* elmt) {
    listobj* current = readyList->pHead;

    if(readyList->pHead->pNext == readyList->pTail) { // If empty list
        readyList->pHead->pNext     = elmt;
        readyList->pTail->pPrevious = elmt;
        elmt->pPrevious             = readyList->pHead;
        elmt->pNext                 = readyList->pTail;
        return OK;
    }

    while((current->pTask->DeadLine < elmt->pTask->DeadLine) && (current != readyList->pTail)) {
        current = current->pNext;
    }

    elmt->pPrevious = current->pPrevious;
    elmt->pNext = current;
    current->pPrevious->pNext = elmt;
    current->pPrevious = elmt;
    return OK;
}