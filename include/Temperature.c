#include <msp.h>
#include <float.h>

#include "Temperature.h"

#define AMBIENT_CAL_20   4065
#define AMBIENT_CAL_32   4405
#define AMBIENT_CAL_DIFF 340

#define MCU_mV_PER_C 1.9
#define MCU_ADC_DIV  6.554
#define MCU_X_OFFSET 687

int32_t MCU_CAL_25T30;
int32_t MCU_CAL_25T85;
int32_t MCU_CAL_DIFF;

int16_t adc_mcu;
int16_t adc_ambient;

int32_t counter = 0;

void ADC14_IRQHandler(void){
    switch (ADC14->IFGR0) {
    case ADC14_IFGR0_IFG30:
        adc_mcu = ADC14->MEM[30];
        break;
    case ADC14_IFGR0_IFG31:
        adc_ambient = ADC14->MEM[31];
        break;
    default:
        break;
    }
    ADC14->IV = 0;
    counter++;
}

double mcu_adc_to_c(double x){
    return ((x-MCU_CAL_25T30)*55)/MCU_CAL_DIFF+30;
}

double ambient_adc_to_c(double x){
    return ((x-AMBIENT_CAL_20)*12)/AMBIENT_CAL_DIFF+20;
}

void temperature_init(Temperatures *self){
    MCU_CAL_25T30 = *((int32_t*)0x002010EC);
    MCU_CAL_25T85 = *((int32_t*)0x002010F0);
    MCU_CAL_DIFF  = MCU_CAL_25T85 - MCU_CAL_25T30;

    P8DIR  &= ~BIT6;
    P8REN  |=  BIT6;
    P8SELC |=  BIT6;

    ADC14->CTL0 &= 0; // Disable ADC

    while (REFCTL0 & REF_A_CTL0_GENBUSY);

    REFCTL0 &= ~REF_A_CTL0_TCOFF; // Enable internal thermistor
    // Set reference voltage for ADC devices and turn on
    REFCTL0 |= (
        REF_A_CTL0_ON     |
        REF_A_CTL0_VSEL_3
    );

    NVIC->ISER[0] = 0x01000000;

    // Pre divide by 64
    // Set to sample timer
    // Divide by 8
    // Set SMCLK source
    // Repeat consecutive channel
    // MSC self trigger
    ADC14->CTL0    |= (
            ADC14_CTL0_PDIV__32      |
            ADC14_CTL0_SHP           |
            ADC14_CTL0_DIV__3        |
            ADC14_CTL0_SHT0__8       |
            ADC14_CTL0_SSEL__SMCLK   |
            ADC14_CTL0_CONSEQ_1      |
            ADC14_CTL0_MSC
    );

    ADC14->IER1  = ADC14_IER1_RDYIE;

    ADC14->IER0 |= ADC14_IER0_IE30 | ADC14_IER0_IE31;

    // Route ADC14TC
    // Use 14 bit precision
    ADC14->CTL1    |= (
        ADC14_CTL1_TCMAP      |
        ADC14_CTL1_RES__14BIT
    );
    ADC14->MCTL[30]  = (
        ADC14_MCTLN_VRSEL_1 |
        22
    );         // Map A22 read to buffer 30
    ADC14->MCTL[31]  = (
        ADC14_MCTLN_EOS |
        19
    );         // Map A19 read to buffer 31
    ADC14->CTL1    |= 0x001E0000; // Starts buffer 0 conversion
    ADC14->CTL0    |= ADC14_CTL0_ON | ADC14_CTL0_ENC | ADC14_CTL0_SC; // Enable and start ADC

    // Remap member functions
    self->read_microprocessor = read_microprocessor;
    self->read_environment    = read_environment;
}

void _read_microprocessor(Temperatures* self){}

void _read_environment(Temperatures* self){}

void read_microprocessor(Temperatures* self){
    self->microprocessor = mcu_adc_to_c(adc_mcu);
    ADC14->CTL0 |= ADC14_CTL0_SC;
//    printf("ADC value: %d\n", adc_mcu);
}

void read_environment(Temperatures* self){
    self->environemnt = ambient_adc_to_c(adc_ambient);
    ADC14->CTL0 |= ADC14_CTL0_SC;
}
