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