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

    // Create an idle task
    create_task(IDLE, UINT_MAX); /// @todo Test return code

    // Set the kernel in startup mode
    Running = task_IDLE;
    LoadContext();

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

