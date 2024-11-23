#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <vector>
#include "mbed.h"
#include "SendingQueue.h"
#include "mail_generated.h"

class SerialCommunication {
public:
    SerialCommunication(int baud_rate);  // Constructor declaration
    void send_struct_via_serial_port(void);

private:
    BufferedSerial m_serial_port;   // Using the correct member name
    int m_baud_rate;

    void send_mail(const SendingQueue::mail_t& mail_data);  // Function to send serialized mail
};

#endif // SERIALCOMMUNICATION_H
