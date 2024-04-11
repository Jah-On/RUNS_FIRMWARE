/*

Header file for drivetrain object

Author: John Schulz
Date:   04/02/2024

*/
#include <stdint.h>

#define DEFAULT_PWM 14999

typedef enum {
    FORWARD = ~0x30,
    REVERSE =  0x30
} drivetrain_direction_t;

typedef enum {
    NONE  = 0x00,
    LEFT  = 0x10,
    RIGHT = 0x20
} drivetrain_rotation_t;

typedef struct Drivetrain_s Drivetrain;

void      _set_speed(struct Drivetrain_s*, int8_t);
void  _set_direction(struct Drivetrain_s*, drivetrain_direction_t);
void   _set_rotation(struct Drivetrain_s*, drivetrain_rotation_t);

struct Drivetrain_s {
    int8_t                 speed;
    drivetrain_direction_t direction;
    drivetrain_rotation_t  rotation;
    void                   (*set_speed)    (struct Drivetrain_s *self, int8_t);
    void                   (*set_direction)(struct Drivetrain_s *self, drivetrain_direction_t);
    void                   (*set_rotation) (struct Drivetrain_s *self, drivetrain_rotation_t);
};

void drivetrain_init(struct Drivetrain_s*);
void       set_speed(struct Drivetrain_s*, int8_t);
void   set_direction(struct Drivetrain_s*, drivetrain_direction_t);
void    set_rotation(struct Drivetrain_s*, drivetrain_rotation_t);
