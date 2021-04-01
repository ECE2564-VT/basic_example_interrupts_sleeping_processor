/*
 * Main.c
 *
 *  Created on: Mar 30, 2021
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhandali
 *
 *  This file contains the entry point of your project, and the interrupt
 *  dispatcher.
 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Our Module Includes - feel free to add more if you need them. */
#include "PollingHAL/Graphics.h"
#include "PollingHAL/SWTimer.h"
#include "InterruptHAL.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/**
 * The main entry point of your project. In this project, you will design an
 * interrupt-driven program which keeps the microcontroller asleep until
 * interrupts occur. When they do occur, you will retrieve the interrupts as
 * necessary and perform the logic you need to implement any user-level logic.
 */
int main(void)
{
    /* Initialize the old system timing module for SWTimers. */
    InitSystemTiming();

    /* GFX struct. Works in the same as it did in the previous projects. */
    GFX gfx = GFX_construct(GRAPHICS_COLOR_BLACK, GRAPHICS_COLOR_WHITE);

    /* Initialize the new interrupt HAL. */
    Init_InterruptHal();

    /* You are free to initialize any other modules, variables, and structs you
     * may potentially need here, especially anything you think may be useful
     * from previous projects. For this example, we decide to turn the left LED
     * on the launchpad on. */
    LaunchpadLED1_TurnOn();
    LaunchpadLED2_TurnOn();

    /* Event handler loop. Unlike the previous two projects, in this project,
     * your microcontroller will sleep until events occur, then perform the
     * proper action by reading a bitfield which tells you what events have
     * occurred since the last time the processor went to sleep. */
    while (true)
    {
        /* DO NOT REMOVE THIS LINE. This puts your microcontroller to sleep
         * until an interrupt occurs. Polling based design approaches will no
         * longer work - this function prevents your processor from polling. */
        SleepProcessor();

        /* Event dispatching logic. Once the processor has awoken, we should
         * check the status of ALL relevant interrupts and perform appropriate
         * logic. In this example, we simply toggle some LEDs, but feel free to
         * replace this with a larger function similar to [Application_loop()]
         * which dispatches multiple events at once. */
        if (LaunchpadS1_Tapped())
            LaunchpadLED2_Toggle();

        /* DO NOT REMOVE THIS IF-STATEMENT.
         * ---------------------------------------------------------------------
         * The non-blocking check in your code. We use this to verify that the
         * event dispatching logic itself is non-blocking - i.e. that anything
         * after the [SleepProcessor()] is non-blocking. */
        if (BoosterpackJS_Tapped())
            LaunchpadLED1_Toggle();
    }
}
