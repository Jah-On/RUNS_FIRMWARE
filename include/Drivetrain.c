#include <msp.h>

#include "Drivetrain.h"
#include <math.h>
#include "Timer.h"

#define ON  0xC0
#define OFF 0x3F

void drivetrain_init(Drivetrain * self){ // Done
    // Port init stuff here
    P2DIR  |=   BIT7 | BIT6; // PWM Controlled
    P2SEL0 |=   BIT7 | BIT6;
    P2SEL1 &= ~(BIT7 | BIT6);

    P3DIR |= BIT7 | BIT6; // nSleep - 0: Off, 1: On
    P5DIR |= BIT5 | BIT4; // PH (Direction) - 0: Forward, 1: Backward

    // Timer init stuff here
    TA0CTL &= CTL_HIGH_RSVD | (
        CTL_TASSEL_SM |
        CTL_DIVIDER_2 |
        CTL_MC_UP     |
        CTL_CLR_FALSE |
        CTL_INTR_OFF  |
        CTL_PEND_NO
    ); // Set 0s
    TA0CTL |= (
        CTL_TASSEL_SM |
        CTL_DIVIDER_2 |
        CTL_MC_UP     |
        CTL_CLR_FALSE |
        CTL_INTR_OFF  |
        CTL_PEND_NO
    ); // Init timer

    TA0CCTL3 = (
        CCTL_CM_NONE        |
        CCTL_CCIS_A         |
        CCTL_SCS_ASYNC      |
        CCTL_SCIS           |
        CCTL_SCCI           |
        CCTL_CAP_COMP       |
        CCTL_OUTMOD_RST_SET |
        CCTL_CCIE_OFF       |
        CCTL_CCI            |
        CCTL_COV_FALSE      |
        CCTL_FLAG_OFF
    ); // Init CCTLs
    TA0CCTL4 = (
        CCTL_CM_NONE        |
        CCTL_CCIS_A         |
        CCTL_SCS_ASYNC      |
        CCTL_SCIS           |
        CCTL_SCCI           |
        CCTL_CAP_COMP       |
        CCTL_OUTMOD_RST_SET |
        CCTL_CCIE_OFF       |
        CCTL_CCI            |
        CCTL_COV_FALSE      |
        CCTL_FLAG_OFF
    );

    TA0CCR0 = 59999;

    // remap struct function calls to real ones
    self->set_speed     = set_speed;
    self->set_direction = set_direction;
    self->set_rotation  = set_rotation;

    self->set_direction(self, self->direction);
    self->set_rotation(self, self->rotation);
}

void _set_speed(Drivetrain *self, int8_t speed){
}

void _set_direction(Drivetrain *self, drivetrain_direction_t direction){
}

void _set_rotation(Drivetrain *self, drivetrain_rotation_t rotation){
}

void set_speed(Drivetrain *self, int8_t speed){ // Done
    if (speed == 0){
        P3OUT &= OFF;
        return;
    }

    self->speed = speed;

    TA0CCR3 = abs(speed) * 599;
    TA0CCR4 = TA0CCR3;

    if (speed > 0){
        self->set_direction(self, FORWARD);
    } else {
        self->set_direction(self, REVERSE);
    }

    P5OUT ^= self->rotation;
    P3OUT |= ON;
}

void set_direction(Drivetrain *self, drivetrain_direction_t direction){ // Done
    switch (direction){
    case FORWARD:
        P5OUT &= FORWARD;
        break;
    case REVERSE:
        P5OUT |= REVERSE;
        break;
    default:
    	return;
    }
    self->direction = direction;
}

void set_rotation(Drivetrain *self, drivetrain_rotation_t rotation){ // Done
    switch (rotation){
    case LEFT:
    case RIGHT:
    case NONE:
        self->set_direction(self, self->direction);
        P5OUT ^= rotation;
        break;
    default:
        return;
    }
    self->rotation = rotation;
}
