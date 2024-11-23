#include <SerialCommunication.h>

// Constructor with initializer list
SerialCommunication::SerialCommunication(int baud_rate)
    : m_serial_port(PB_6, PB_7), m_baud_rate(baud_rate) {  // Corrected to match member
    m_serial_port.set_baud(m_baud_rate);
    m_serial_port.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::Odd,
        /* stop bit */ 1
    );
}

// Function to send the serialized data over UART
void SerialCommunication::send_mail(void) {
    auto buffer = serialize_mail();
    // Send the data over BufferedSerial
    m_serial_port.write(buffer.data(), buffer.size());
}

// Function to continuously send data from the mailbox
void SerialCommunication::send_struct_via_serial_port(void) {
    // Access the shared queue
    SendingQueue& sending_queue = SendingQueue::getInstance();

    while (true) {
        osEvent evt = sending_queue.mail_box.get();
        if (evt.status == osEventMail) {
            SendingQueue::mail_t *sending_mail = (SendingQueue::mail_t *)evt.value.p;
            send_mail();  // Fixed missing semicolon
            sending_queue.mail_box.free(sending_mail); 
        }

        // You can add a delay or a condition to break the loop if needed
        ThisThread::sleep_for(100);  // Sleep to prevent unnecessary CPU usage
    }

    printf("Data sent via serial\n");

    return;  // Returning from the function, but this is unreachable if while(true)
}
