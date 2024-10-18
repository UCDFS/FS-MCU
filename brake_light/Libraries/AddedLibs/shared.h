/**********************************************************************************************************************
 * \file shared.h
 *********************************************************************************************************************/

#ifndef LIBRARIES_ADDEDLIBS_SHARED_H_
#define LIBRARIES_ADDEDLIBS_SHARED_H_

// Constant definitions
#define BRAKE_PRESSURE_THRESHOLD  50  // Adjust as needed

// Pin definitions
#define BRAKE_LIGHT_PIN  1  // Adjust as needed
#define BRAKE_PEDAL_PIN  0  // ADC channel for brake pressure sensor

// Includes
#include "Ifx_Types.h"
#include "IfxEvadc_Adc.h"
#include "IfxPort.h"

// Global variables
extern volatile int globalBrakePressure;  // Shared variable for brake pressure

// Function prototypes
void initGpio(void);          // Initializes GPIO for brake light control
void turnOnBrakeLight(void);  // Turns on the brake light
void turnOffBrakeLight(void); // Turns off the brake light

void initAdc(void);           // Initializes ADC for brake pressure sensor
int readBrakePressure(void);  // Reads the current brake pressure value

// ADC variables (declare extern if used in multiple files)
extern IfxEvadc_Adc evadc;
extern IfxEvadc_Adc_Group adcGroup;
extern IfxEvadc_Adc_Channel adcChannel;

#endif /* LIBRARIES_ADDEDLIBS_SHARED_H_ */
