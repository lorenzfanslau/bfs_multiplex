#include "core/core.h"
#include "vector"
#include "multiplexer.h"
#include "flight/msg.h"

namespace bfs {

Multiplexer::Multiplexer() : SLAVE_ADDR(0x78), TCAADDR(0x70) {}   //constant i2C adresses of both multiplexer (x70) and HCLA pressure sensors(x78)

bool Multiplexer::Begin(int baudrate) {
    Wire.begin();
    count=0;
    Serial.begin(baudrate);
    if (setup()) {
        return true;
    }
    return false; // if setup goes wrong
}

bool Multiplexer::setup() {
    for (int i = 0; i <= 7; i++) {
        if (tcaselect(i)) {
            float wert=std::abs(GetPressure(SLAVE_ADDR));
            if ( wert < 5) {        //check if value of sensor is in bound, if no sensor is connected to the multiplexer, the if-case will be ignored 
                //MsgInfo("werte der sensoren: ");
                availableSensors.push_back(i);   //list of recognized sensors from multiplexer board gets extended
                //char bummel[20];
                //dtostrf(wert, 6, 2, bummel);
                //MsgInfo(bummel);
                //MsgInfo("\n");
            }
        }
    }
    printAvailableSensors();        //useful for user interface
    return true;
}

void Multiplexer::printAvailableSensors() {
    MsgInfo("sensoren auf multiplexer: \n");
    for (int sensorIndex : availableSensors) {    //index of available sensors will be displayed
        char buffer[20];
        sprintf(buffer,"%d", sensorIndex);
        MsgInfo(buffer);
    }
    Serial.println();
}

bool Multiplexer::tcaselect(uint8_t i) {   //method to differ between identical sensors connected to the multiplexer
    if (i > 7)                              // maximum of eight sensors (usually four)
        return false;
    Wire.beginTransmission(TCAADDR);       //start I2C communication with multiplexer device
    Wire.write(1 << i);
    int error = Wire.endTransmission();
    return (error == 0);
}

bool Multiplexer::Read(KegelsondeData *const ptr) {
    if (!ptr) {
        return false;
    }
    ptr->new_data = true; // braucht noch Arbeit
    ptr->healthy = true; //(health_timer_ms_ < health_period_ms_);        ???
    if (ptr->new_data) {
        for (int sensorIndex : availableSensors) {
            tcaselect(sensorIndex);
            //ptr->pres_mbar.push_back(GetPressure(SLAVE_ADDR));      //don't know the right solution yet 
            ptr->pres_mbar[sensorIndex] = GetPressure(SLAVE_ADDR);
        }
        // health_timer_ms_ = 0;
    }
    return ptr->new_data;
}

float Multiplexer::GetPressure(int addr) {    //method to read the actual pressure values
    float result;
    unsigned int value;
    Wire.beginTransmission(addr);
    Wire.requestFrom(addr, 1);
    value = Wire.read();
    value <<= 8;
    result = ((float(value) - 1638) / 5253) - 2.5;    //calculate the result
    return result;
}

} // Ende namespace
