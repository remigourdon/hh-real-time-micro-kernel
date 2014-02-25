/**
 * @file kernel.c
 * @brief Kernel declarations
 */

#include "kernel.h"
#include "lists.h"

exception init_kernel(void) {
    set_ticks(0);   // Set tick counter to zero

    // Create necessary data structures
    readyList = create_emptyList();
    timingList = create_emptyList();
    waitingList = create_emptyList();

    // Create an idle task
    create_task(IDLE, UINT_MAX);

    // Set the kernel in startup mode
    Running = task_IDLE;
    LoadContext();

    // Return status
    return OK;
}