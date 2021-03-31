/*
 * InterruptHAL.c
 *
 *  Created on: Mar 30, 2021
 *      Author: Matthew Zhong
 *  Supervisor: Leyla Nazhandali
 *
 *  In this file, you'll implement the majority (if not all) of your hardware
 *  interfacing. Any implementation work involving TIMER32_1, TIMER_A, and GPIOs
 *  should be written in this file. This is to keep a clean modular approach
 *  which abstracts the hardware implementation to this dedicated file.
 *
 *  You'll note that we make use of static functions and static file-scope
 *  variables.
 *
 *  - Static functions are typically functions which are only accessible in this
 *    file. As an example, you won't be able to call [Init_LaunchpadLEDs()]
 *    anywhere except in this file.
 *
 *  - Similarly, static file-scope variables are variables which behave like
 *    global variables but which are only accessible in this file. For example,
 *    you can access any of the variables inside of the [s_hal] struct in this
 *    file from any function inside this file, but you can't access the members
 *    of [s_hal] from [Main.c]. ISRs require either globals or static variables,
 *    so to reduce global variable access to a minimum, we opt to make static
 *    file-scope variables.
 */

#include <InterruptHAL.h>
#include <PollingHAL/SWTimer.h>

/******************************************************************************/
/* INTERRUPT HAL STRUCT DEFINITION                                            */
/******************************************************************************/

/**
 * An Interrupt HAL, used to manage interrupt inputs whose events must be
 * processed with either global or static variables. Any variable which is
 * modified in an ISR MUST be declared as volatile, or else you risk the
 * variable being optimized away upon compilation.
 */
struct _InterruptHAL
{
    /*
     * An event bitfield variable whose bits are to be written a 1 (as
     * appropriate) whenever a corresponding ISR event occurs. When the main
     * application reads the event bitfield, we then reset this variable so that
     * we're ready to log more events from ISRs again.
     */
    volatile HAL_Event Event;
};
typedef struct _InterruptHAL InterruptHAL;

/* The single instance of our InterruptHAL. We declare this as a file-scope
 * static variable - this is a little better than a global variable since we can
 * only modify and read the values of s_hal inside of InterruptHAL.c and NOT in
 * any other files. */
static InterruptHAL s_hal;

/******************************************************************************/
/* STATIC FUNCTION HEADERS AND PREPROCESSOR MACROS                            */
/******************************************************************************/
#define DEBOUNCE_TIME_MS            (300)

/* Interrupt Service Routines ----------------------------------------------- */
static void ISR_LaunchpadButtons(void);
/* TODO: You will most likely need to add more interrupt service routines as  */
/*       you expand what hardware you need to use from the board.             */
/* -------------------------------------------------------------------------- */

/* Initialization Functions ------------------------------------------------- */
/* TODO: You will most likely need to add more initialization functions as    */
/*       you expand what hardware you need to use from the board.             */
/* -------------------------------------------------------------------------- */
static void Init_HALVariables(void);
static void Init_LaunchpadLEDs(void);
static void Init_LaunchpadButtons(void);

/******************************************************************************/
/* INTERRUPT SERVICE ROUTINES (ISRS)                                          */
/******************************************************************************/

/**
 * Automatically invoked by the MSP432's interrupt controller whenever any
 * input pin on GPIO_PORT1 triggers an interrupt event. Do not call this
 * function manually.
 */
static void ISR_LaunchpadButtons(void)
{
    /* Debounce state variables and first-call initialization. Note - the     */
    /* SWTimer module has been updated as of March 30, 2021 so that           */
    /* SWTimer_construct() returns timers which are already expired. As a     */
    /* direct result, when this function is first called (probably when       */
    /* pressing L1 for the first time after reset), the if-statement which    */
    /* checks if the timer expired evaluates as TRUE, allowing the button     */
    /* event to be logged even on the first trigger of this ISR.              */
    /* ---------------------------------------------------------------------- */
    static SWTimer debounceL1;
    static bool firstCall = false;

    if (firstCall)
    {
        debounceL1 = SWTimer_construct(DEBOUNCE_TIME_MS);
        firstCall = false;
    }

    /* First, determine which pins triggered this ISR. Different pins may     */
    /* require different system responses from your ISR.                      */
    /* ---------------------------------------------------------------------- */
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);

    /* Check if L1 (Port 1, Pin 1) generated this ISR. */
    if ((status & GPIO_PIN1) == GPIO_PIN1)
    {
        /* Log the event in our event bitmask if the debouncer has expired */
        if (SWTimer_expired(&debounceL1))
        {
            s_hal.Event |= HAL_L1_TAPPED;

            /* FOR DEMO PURPOSES ONLY - You can remove this. For debugging    */
            /* purposes, you can turn on and off LEDs in ISRs as appropriate. */
            LaunchpadLED2_Toggle();

            /* Restart this timer so that if the interrupt triggers again too */
            /* soon after this call, we ignore it until the timer expires.    */
            SWTimer_start(&debounceL1);
        }

        /* After servicing an interrupt, clear appropriate interrupt flags. */
        GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    }
}

/**
 * Initializes the variables inside of the HAL struct.
 *
 * TODO: Extend this function as you inevitably add more variables to the
 *       interrupt HAL.
 */
static void Init_HALVariables()
{
    s_hal.Event = HAL_NONE;
}

/**
 * Initializes each of the LEDs using the GPIO driverlib. Rather than using the
 * old LED structs and HAL, this version specifically allows you to turn on and
 * off LEDs without the need to specify a parameter in the function calls,
 * meaning you can use these calls in an ISR to help debug your code.
 */
static void Init_LaunchpadLEDs(void)
{
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0); /* LED1     */
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0); /* LED2 Red */

    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0);
}

/**
 * Initializes Launchpad S1 (L1) with interrupts enabled on a high-to-low
 * transition, or a falling edge.
 *
 * TODO: Add Launchpad S2 (L2) initialization to this, and tweak or add to the
 *       ISRs to generate events upon pressing L2 as well.
 */
static void Init_LaunchpadButtons(void)
{
    /* Launchpad L1 (P1.1) is an input with a pull-up resistor required. */
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    /* Enable interrupts for Launchpad L1 through GPIO Port 1 */
    GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_registerInterrupt(GPIO_PORT_P1, ISR_LaunchpadButtons);
    GPIO_interruptEdgeSelect(
        GPIO_PORT_P1, GPIO_PIN1, GPIO_HIGH_TO_LOW_TRANSITION);

    /* Enables the interrupt for GPIO_PORT1 events. To determine what other
     * events are available for configuration, CTRL+click on INT_PORT1. */
    Interrupt_enableInterrupt(INT_PORT1);
}

/******************************************************************************/
/* PUBLIC-FACING FUNCTIONS (callable outside of this file)                    */
/******************************************************************************/

/** Basic manipulation - turns on LED1. */
void LaunchpadLED1_TurnOn(void)
{ GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0); }

/** Basic manipulation - turns on LED2. */
void LaunchpadLED2_TurnOn(void)
{ GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN0); }

/** Basic manipulation - turns off LED1. */
void LaunchpadLED1_TurnOff(void)
{ GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0); }

/** Basic manipulation - turns off LED2. */
void LaunchpadLED2_TurnOff(void)
{ GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0); }

/** Basic manipulation - toggles LED1. */
void LaunchpadLED1_Toggle(void)
{ GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0); }

/** Basic manipulation - toggles LED2. */
void LaunchpadLED2_Toggle(void)
{ GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN0); }

/**
 * Interrupt HAL initialization function. This function calls all the other
 * initialization functions for other hardware modules as needed - feel free to
 * extend this function as necessary.
 */
void Init_InterruptHal(void)
{
    /* First, disable all interrupts again. */
    Interrupt_disableMaster();

    /* Static variable initialization */
    Init_HALVariables();

    /* Input peripheral initialization */
    Init_LaunchpadButtons();

    /* Output initialization */
    Init_LaunchpadLEDs();

    /* Allows the microcontroller to wake up and return from PCM_gotoLPM0()
     * after an ISR is fired. (Depending on the interrupt, in order to wake the
     * processor, you may also need to manually disable the corresponding ISR by
     * calling [Interrupt_disableInterrupt()] with the corresponding interrupt
     * number.
     */
    Interrupt_disableSleepOnIsrExit();
    Interrupt_enableMaster();
}

/**
 * Retrieves the most recent event(s) generated by a relevant ISR. In order to
 * return multiple events, we bitmask the events in the HAL_Event member of
 * s_hal. This allows us to return multiple values while only using a single
 * return variable.
 *
 * For example, if both L1 and L2 were pressed at the exact same time, the
 * HAL_Event we return will be the bitwise OR as follows:
 *
 *   s_hal.Event == HAL_L1_TAPPED | HAL_L2_TAPPED
 *
 * This means that to check what events have occurred since the last time the
 * processor was put to sleep, you can simply use the individual bitmasks as
 * follows:
 *
 * [code]
 *   HAL_Event event = InterruptHAL_MostRecentEvent()
 *
 *   if (event & HAL_L1_TAPPED)
 *   {
 *       // your code goes here
 *   }
 *
 *   if (event & HAL_L2_TAPPED)
 *   {
 *       // your code goes here
 *   }
 * [/code]
 */
HAL_Event InterruptHAL_MostRecentEvent(void)
{
    HAL_Event event = s_hal.Event;
    s_hal.Event = HAL_NONE;

    return event;
}
