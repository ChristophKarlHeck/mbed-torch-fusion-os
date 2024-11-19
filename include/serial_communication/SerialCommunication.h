#include <vector>
#include "mbed.h"

class SerialCommunication{
    public:
        SerialCommunication(int baud_rate);
        int send_model_output(std::vector<float> model_output);

    private:

        int m_baud_rate;
        uint8_t m_bufbuf[9]; // Buffer for data to be sent
         // CN10 pins on P Nucleo WB55RG

};
