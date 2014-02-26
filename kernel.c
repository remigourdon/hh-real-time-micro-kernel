/**
 * @file kernel.c
 * @brief Kernel declarations
 */

#include "kernel.h"

exception init_kernel(void) {
    set_ticks(0);   // Set tick counter to zero

    // Create necessary data structures
    readyList = create_emptyList();
    timingList = create_emptyList();
    waitingList = create_emptyList();

    // Create IDLE task
    TCB* task_IDLE;

    /// BEGIN CRITICAL ZONE
    isr_off();
    task_IDLE = (TCB*)malloc(sizeof(TCB));
    isr_on();
    /// END CRITICAL ZONE

    task_IDLE->DeadLine = UINT_MAX;
    task_IDLE->PC = IDLE;
    task_IDLE->SP = &(task_IDLE->StackSeg[STACK_SIZE-1]);
    insert_readyList(task_IDLE);

    // Set the kernel in startup mode
    MODE = INIT;

    // Return status
    return OK;
}

exception create_task(void (*body)(), uint deadline) {
    // Allocate memory for TCB
    TCB* newTask;

    // BEGIN CRITICAL ZONE
    isr_off();
    newTask = (TCB*)malloc(sizeof(TCB));
    isr_on();
    // END CRITICAL ZONE

    // Set deadline in TCB
    newTask->DeadLine = deadline;

    // Set TCB's PC to point to the task body
    newTask->PC = body;

    // Set TCB's SP to point to the task segment
    newTask->SP = &(newTask->StackSeg[STACK_SIZE-1]);
}

