/**
 * @file lists.h
 * @brief List management system definition.
 */

#ifndef LISTS_H
#define LISTS_H

/////////////////////
// LIST STRUCTURES //
/////////////////////

/**
 * @brief Generic list item.
 */
typedef struct l_obj {
    TCB             *pTask;     /**< Pointer to the task function */
    uint            nTCnt;      /**< Tick counter */
    msg             *pMessage;
    struct l_obj    *pPrevious; /**< Pointer to previous object in list */
    struct l_obj    *pNext;     /**< Pointer to next object in list */
} listobj;

/**
 * @brief Generic list.
 */
typedef struct {
    listobj         *pHead;     /**< Pointer to the head of the list */
    listobj         *pTail;     /**< Pointer to the tail of the list */
} list;

#endif