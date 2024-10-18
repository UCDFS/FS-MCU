/*
 * \file Cpu0_Main.c
 * 
*/

#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "IfxEvadc_Adc.h"
#include "IfxPort.h"
#include "IfxStm.h"
#include "shared.h"  // Include shared header for global variables and functions

// Define the global variables here
IfxEvadc_Adc evadc;
IfxEvadc_Adc_Group adcGroup;
IfxEvadc_Adc_Channel adcChannel;

volatile int globalBrakePressure = 0; // Define the global variable


IFX_ALIGN(4) IfxCpu_syncEvent g_cpuSyncEvent = 0;

void initAdc(void) {
    // Create and initialize the EVADC module configuration
    IfxEvadc_Adc_Config adcConfig;
    IfxEvadc_Adc_initModuleConfig(&adcConfig, &MODULE_EVADC);
    IfxEvadc_Adc_initModule(&evadc, &adcConfig);

    // Create and initialize the group configuration
    IfxEvadc_Adc_GroupConfig adcGroupConfig;
    IfxEvadc_Adc_initGroupConfig(&adcGroupConfig, &evadc);

    // Set the group ID (e.g., group 0)
    adcGroupConfig.groupId = IfxEvadc_GroupId_0;
    adcGroupConfig.master  = adcGroupConfig.groupId;

    // Enable the queue request source and set the gating mode
    adcGroupConfig.arbiter.requestSlotQueue0Enabled = TRUE;
    adcGroupConfig.queueRequest[0].triggerConfig.gatingMode = IfxEvadc_GatingMode_always;

    // Initialize the group
    IfxEvadc_Adc_initGroup(&adcGroup, &adcGroupConfig);

    // Create and initialize the channel configuration
    IfxEvadc_Adc_ChannelConfig adcChannelConfig;
    IfxEvadc_Adc_initChannelConfig(&adcChannelConfig, &adcGroup);

    // Set the channel ID (e.g., channel corresponding to BRAKE_PEDAL_PIN)
    adcChannelConfig.channelId = (IfxEvadc_ChannelId)BRAKE_PEDAL_PIN;
    adcChannelConfig.resultRegister = IfxEvadc_ChannelResult_0; // Use result register 0

    // Initialize the channel
    IfxEvadc_Adc_initChannel(&adcChannel, &adcChannelConfig);

    // Add the channel to the queue with refill enabled
    IfxEvadc_Adc_addToQueue(&adcChannel, IfxEvadc_RequestSource_queue0, IFXEVADC_QUEUE_REFILL);

    // Start the queue
    IfxEvadc_Adc_startQueue(&adcGroup, IfxEvadc_RequestSource_queue0);
}

int readBrakePressure(void) {
    Ifx_EVADC_G_RES conversionResult;

    // Wait for a valid result
    do {
        conversionResult = IfxEvadc_Adc_getResult(&adcChannel);
    } while (!conversionResult.B.VF); // Wait until the valid flag is set

    return conversionResult.B.RESULT;
}

void core0_main(void)
{
    // Wait for all CPUs to start up
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    IfxCpu_enableInterrupts();
    
    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());
    
    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    /* Initialize ADC for brake pressure sensor */
    initAdc();  // Initializes the ADC channel for brake pressure sensor

    while (1)
       {

            // Read the brake pressure value from the ADC
            int pressure = readBrakePressure();

            // Update the shared variable with a memory barrier
            __dsync(); // Ensure all previous writes are completed
            globalBrakePressure = pressure;
            __isync(); // Ensure the write is visible to other cores


           // Add a small delay (e.g., 10ms)
           IfxStm_waitTicks(&MODULE_STM0, IfxStm_getTicksFromMilliseconds(&MODULE_STM0, 10));
       }
}

