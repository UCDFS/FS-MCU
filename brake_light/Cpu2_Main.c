/*
 * \file Cpu2_Main.c
*/

#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "shared.h"  // Include shared header for global variables and GPIO functions

extern IfxCpu_syncEvent g_cpuSyncEvent;

void initGpio(void) {
    // Set the brake light pin as output
    IfxPort_setPinModeOutput(&MODULE_P10, BRAKE_LIGHT_PIN, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
}

void turnOnBrakeLight(void) {
    // Set the output high to turn on the brake light
    IfxPort_setPinHigh(&MODULE_P10, BRAKE_LIGHT_PIN);
}

void turnOffBrakeLight(void) {
    // Set the output low to turn off the brake light
    IfxPort_setPinLow(&MODULE_P10, BRAKE_LIGHT_PIN);
}

void core2_main(void)
{
    IfxCpu_enableInterrupts();
    
    /* !!WATCHDOG2 IS DISABLED HERE!! Enable the watchdog if required */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    
    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    /* Initialize GPIO for brake light control */
    initGpio();  // Initializes the GPIO pin for the brake light

    while (1)
    {

            // Use memory barriers when reading the shared variable
            __dsync(); // Ensure all previous reads are completed
            int pressure = globalBrakePressure;
            __isync(); // Ensure subsequent reads get the latest value

            // Control the brake light based on the pressure
            if (pressure > BRAKE_PRESSURE_THRESHOLD)
            {
                turnOnBrakeLight();
            }
            else
            {
                turnOffBrakeLight();
            }

    }
}

