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
    /* Stop the Watchdog timer. */
    WDT_A_holdTimer();

    /* GFX struct. Works in the same as it did in the previous projects. */
    GFX gfx = GFX_construct(GRAPHICS_COLOR_BLACK, GRAPHICS_COLOR_WHITE);
    InitSystemTiming();

    /* Initialize the new interrupt HAL. */
    Init_InterruptHal();

    /* You are free to initialize any other modules, variables, and structs you
     * may potentially need here, especially anything you think may be useful
     * from previous projects. */
    LaunchpadLED1_TurnOn(); /* Dummy function call - replace as needed. */

    /* Event handler loop. Unlike the previous two projects, in this project,
     * your microcontroller will sleep until events occur, then perform the
     * proper action by reading a bitfield which tells you what events have
     * occurred since the last time the processor went to sleep.
     */
    while (true)
    {
        /* DO NOT REMOVE THIS LINE. This puts your microcontroller to sleep
         * until an interrupt occurs. Polling based design approaches will no
         * longer work - this function prevents your processor from polling. */
        PCM_gotoLPM0();

        /* Retrieve a bitfield of the most recent events triggered from ISRs. */
        HAL_Event event = InterruptHAL_MostRecentEvent();

        /* A simple example of how we can interpret the event - just turn an LED
         * on. In your solution, you will probably write something more
         * complicated - feel free to call functions which implement your logic
         * as necessary. */
        if (event & HAL_L1_TAPPED)
            LaunchpadLED1_Toggle();
    }
}
