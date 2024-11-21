#include "mbed.h"

class SendingPayload{
   /* 
    * Mail: 
    * 
    * This Mail structure is used for Inter-Thread Communication between the 
    * reading data thread and the main thread. It facilitates the exchange of 
    * data between threads in a safe and efficient manner.
    */
    typedef struct {
        std::vector<std::array<uint8_t,3>> inputs; /* Vector of downsampled analog values*/
        bool channel;
    } mail_t;


    Mail<mail_t, 4> mail_box; // size has to be the same as model_input_size
};
