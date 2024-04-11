#include <stdint.h>

typedef struct Temperatures_s Temperatures;

struct Temperatures_s {
    int8_t microprocessor;
    int8_t environemnt;
    void   (*read_microprocessor)(struct Temperatures_s*);
    void   (*read_environment)(struct Temperatures_s*);
};

void temperature_init(Temperatures*);
void _read_microprocessor(Temperatures*);
void _read_environment(Temperatures*);
void read_microprocessor(Temperatures*);
void read_environment(Temperatures*);
