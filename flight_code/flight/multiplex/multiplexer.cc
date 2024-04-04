#include "Wire.h"
#include "vector"

namespace bfs
{

    class Multiplexer
    {
    public:
        Multiplexer() : SLAVE_ADDR(0x78), TCAADDR(0x70) {}

        bool Begin(int baudrate = 19200){
            Wire.begin();
            Serial.begin(baudrate);
            if (setup())
            {
                return true;
            }
        }

        bool setup(){
            for (int i = 0; i <= 7; i++)
            {
                if (tcaselect(i))
                {
                    if (std::abs(GetPressure(SLAVE_ADDR)) < 5)
                    {
                        availableSensors.push_back(i);
                    }
                }
            }
            return true;
        }

        void printAvailableSensors(){
            Serial.println("Available sensors:");
            for (int sensorIndex : availableSensors)
            {
                Serial.print(sensorIndex);
                Serial.print(" ");
            }
            Serial.println();
        }

    private:
        const int SLAVE_ADDR;
        const int TCAADDR;
        std::vector<int> availableSensors;

        bool tcaselect(uint8_t i){
            if (i > 7)
                return false;
            Wire.beginTransmission(TCAADDR);
            Wire.write(1 << i);
            int error = Wire.endTransmission();
            return (error == 0);
        }

        bool Read(Data *const ptr){
            if (!ptr)
            {
                return false;
            }
            ptr->new_data = true; // braucht noch Arbeit
            // ptr->healthy = (health_timer_ms_ < health_period_ms_);        ???
            if (ptr->new_data)
            {
                for (int sensorIndex : availableSensors)
                {
                    tcaselect(sensorIndex);
                    ptr->pres_mbar.push_back(GetPressure(SLAVE_ADDR))
                }
                // health_timer_ms_ = 0;
            }
            return ptr->new_data;
        }

        float GetPressure(int addr){
            float result;
            unsigned int value;
            Wire.beginTransmission(addr);
            Wire.requestFrom(addr, 1);
            value = Wire.read();
            value <<= 8;
            result = ((float(value) - 1638) / 5253) - 2.5;
            return result;
        }
    };
} // Ende namespace
