#include <SerialCommunication.h>

// Constructor with initializer list
SerialCommunication::SerialCommunication(int baud_rate)
    : m_baud_rate(baud_rate){
    
    // m_serial_port.set_baud(m_baud_rate);
    // m_serial_port.set_format(
    //     /* bits */ 8,
    //     /* parity */ BufferedSerial::None,
    //     /* stop bit */ 1
    // );
}

int SerialCommunication::send_model_output(std::vector<float> model_output) {

    printf("hi\n");

    uint8_t m_bufbuf[9];

    m_bufbuf[0] = 80;    // Start marker
    m_bufbuf[1] = 0x01;  // Board identifier (e.g., board #1)

    // Serialize up to 2 floats into the buffer
    for (size_t i = 0; i < std::min<size_t>(2, model_output.size()); ++i) {
        uint32_t value = *reinterpret_cast<const uint32_t*>(&model_output[i]); // Treat float as uint32_t
        m_bufbuf[2 + i * 3] = (value >> 16) & 0xFF; // High byte
        m_bufbuf[3 + i * 3] = (value >> 8) & 0xFF;  // Mid byte
        m_bufbuf[4 + i * 3] = value & 0xFF;         // Low byte
    }

    m_bufbuf[8] = 0x0A; // End marker

    // Send the serialized buffer
    // m_serial_port.write(m_bufbuf, sizeof(m_bufbuf));

    printf("Data sent via serial\n");

    return 1;
}
