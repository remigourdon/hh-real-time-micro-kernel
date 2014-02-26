/**
 * @file lists.h
 * @brief List management system definition.
 */

#ifndef LISTS_H
#define LISTS_H

#include "kernel.h"

/////////////////////
// LIST STRUCTURES //
/////////////////////

/**
 * @brief Generic list item.
 */
typedef struct l_obj {
    TCB             *pTask;     /**< Pointer to the task function. */
    uint            nTCnt;      /**< Tick counter. */
    msg             *pMessage;
    struct l_obj    *pPrevious; /**< Pointer to previous object in list. */
    struct l_obj    *pNext;     /**< Pointer to next object in list. */
} listobj;

/**
 * @brief Generic list.
 */
typedef struct {
    listobj         *pHead;     /**< Pointer to the head of the list. */
    listobj         *pTail;     /**< Pointer to the tail of the list. */
} list;

///////////////////////////////
// FUNCTIONS TO MANAGE LISTS //
///////////////////////////////

/**
 * @brief Create a new empty list and return it.
 *
 * Initializes pHead and pTail.
 *
 * @return  Pointer to the new empty list.
 */
list* create_emptyList(void);

/**
 * @brief Insert a new element in the readyList.
 *
 * The list is sorted from the lowest deadline values first.
 *
 * @param elmt Pointer to the element to be inserted.
 * @return     OK if success, FAIL otherwise.
 */
exception insert_readyList(listobj* elmt);

/**
 * @brief Extract an element from the readyList.
 *
 * The first element of the list (lowest deadline) is extracted.
 *
 * @return Pointer to the extracted element or NULL.
 */
listobj* extract_readyList(void);

/**
 * @brief Insert a new element in the timerList.
 *
 * The list is sorted from the lowest number of ticks values first.
 *
 * @param elmt  Pointer to the element to be inserted.
 * @param nTCnt Number of ticks to wait.
 * @return      OK if success, FAIL otherwise.
 */
exception insert_timerList(listobj* elmt, uint nTCnt);

/**
 * @brief Extract an element from the timerList.
 *
 * The first element of the list (lowest tick counter) is extracted.
 *
 * @return  Pointer to the extracted element or NULL.
 *
 */
listobj* extract_timerList(void);

/**
 * @brief Insert a new element in the waitingList.
 *
 * The list is sorted from the lowest deadline values first.
 *
 * @param elmt Pointer to the element to be inserted.
 * @return     OK if success, FAIL otherwise.
 */
exception insert_waitingList(listobj* elmt);

/**
 * @brief Extract an element from the waitingList.
 *
 * The element corresponding to the parameter is extracted.
 *
 * @param  elmt Pointer to the element to be extracted.
 * @return      Pointer to the extracted element or NULL.
 */
listobj* extract_waitingList(listobj* elmt);

#endif