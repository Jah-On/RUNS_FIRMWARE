#include <msp.h>

#include "Bumper.h"
#include "Drivetrain.h"

extern Bumper bumper;

extern Drivetrain drivetrain;

void bumper_init(){
    // Port configuration
    P4DIR &=  0x12;
    P4REN |= ~0x12;
    P4OUT |= ~0x12;

    // Interrupt configuration
    P4IE  |= ~0x12;
    P4IES |= ~0x12;
    P4IFG &=  0x12;

    // NVIC configuration
    NVIC->ISER[1] = BIT6;
}

void PORT4_IRQHandler(){
    switch (P4IV){
    case 0x02:
        bumper ^= BIT0;
        P4IES  ^= BIT0;
        break;
    case 0x06:
        bumper ^= BIT1;
        P4IES  ^= BIT2;
        break;
    case 0x08:
        bumper ^= BIT2;
        P4IES  ^= BIT3;
        break;
    case 0x0C:
        bumper ^= BIT3;
        P4IES  ^= BIT5;
        break;
    case 0x0E:
        bumper ^= BIT4;
        P4IES  ^= BIT6;
        break;
    case 0x10:
        bumper ^= BIT5;
        P4IES  ^= BIT7;
        break;
    default:
        break;
    }
    P4IFG &= 0x12;
    if (bumper && (drivetrain.speed > 0)){
        drivetrain.set_speed(&drivetrain, 0);
    }
}
