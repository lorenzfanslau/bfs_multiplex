#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

//#include <Wire.h>
#include "vector"
#include "flight/kegelsonde.h"

namespace bfs{

class Multiplexer {

public:
    Multiplexer();
    bool Begin(int baudrate = 19200);
    bool setup();
    void printAvailableSensors();
    bool Read(KegelsondeData *const ptr);

private:
    const int SLAVE_ADDR;
    const int TCAADDR;
    std::vector<int> availableSensors;

    bool tcaselect(uint8_t i);
    
    float GetPressure(int addr);
};
}
#endif
