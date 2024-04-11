#include <msp.h>

#include "../include/AP.h"
#include "../include/Bumper.h"
#include "../include/Clock.h"
#include "../include/CortexM.h"
#include "../include/Drivetrain.h"
#include "../include/Temperature.h"
#include "../include/UART0.h"
#include "../include/UART1.h"

void mcu_temp_read_callback();
void ambient_temp_read_callback();
void bumper_read_callback();
void velocity_write_callback();
void rotation_write_callback();

typedef struct WriteBufs {
    int8_t velocity;
    int8_t rotation;
} WriteBufs;

Temperatures temperatures = {
    0,
    0,
    _read_microprocessor,
    _read_environment
};
Drivetrain   drivetrain = {
    0,
    FORWARD,
    NONE,
    _set_speed,
    _set_direction,
    _set_rotation
};
Bumper    bumper;
WriteBufs bt_bufs;

// Temporary, delete later!
extern int32_t MCU_CAL_25T30;
extern int32_t MCU_CAL_25T85;

void add_temperature_service(){
    AP_AddService(0xA000);

    AP_AddCharacteristic(0xA001,1,&temperatures.microprocessor,0x01,0x02,"MCU",    &mcu_temp_read_callback,    0);
    AP_AddCharacteristic(0xA002,1,&temperatures.environemnt   ,0x01,0x02,"Ambient",&ambient_temp_read_callback,0);

    AP_RegisterService();
}

void mcu_temp_read_callback(){
    temperatures.read_microprocessor(&temperatures);
}

void ambient_temp_read_callback(){
    temperatures.read_environment(&temperatures);
}

void add_proximity_service(){
    AP_AddService(0xB000);

    AP_AddCharacteristic(0xB001,1,&bumper,0x01,0x02,"Bumper",&bumper_read_callback,0);

    AP_RegisterService();
}

void bumper_read_callback(){
}

void add_movement_service(){
    AP_AddService(0xC000);

    AP_AddCharacteristic(0xC001,1,&bt_bufs.velocity,0x02,0x08,"Velocity",0,&velocity_write_callback);
    AP_AddCharacteristic(0xC002,1,&bt_bufs.rotation,0x02,0x08,"Rotation",0,&rotation_write_callback);

    AP_RegisterService();
}

void velocity_write_callback(){
    if ((bt_bufs.velocity > 100) || (bt_bufs.velocity < -100)){
        return;
    }

    drivetrain.set_speed(&drivetrain, bt_bufs.velocity);
}

void rotation_write_callback(){
    drivetrain.set_rotation(&drivetrain, bt_bufs.rotation);
}

void main(void){
    DisableInterrupts();
    Clock_Init48MHz();
    UART0_Init();

    temperature_init(&temperatures);

    bumper_init();
    drivetrain_init(&drivetrain);

    EnableInterrupts();

    AP_Init();
    AP_GetStatus();  // optional
    AP_GetVersion(); // optional

    add_temperature_service();
    add_proximity_service();
    add_movement_service();

    AP_StartAdvertisementJacki();
    AP_GetStatus(); // optional

loop:

//    temperatures.read_microprocessor(&temperatures);
//    Clock_Delay1ms(500);
    AP_BackgroundProcess();  // handle incoming SNP frames

    goto loop;
}
