#include "serial_mail_sender/SerialMailSender.h"

#define BAUDRATE 115200

// Initialize the static BufferedSerial instance (PC_1 = TX, PC_0 = RX)
// Raspberry Pi: (GPIO 14 = TX, GPIO 15 = RX)
// Connection TX-RX, RX-TX, GND-GND
BufferedSerial SerialMailSender::m_serial_port(PC_1, PC_0, BAUDRATE);

// Get the single instance of SerialMailSender
SerialMailSender& SerialMailSender::getInstance(void) {
    static SerialMailSender instance;
    return instance;
}

// Private constructor
SerialMailSender::SerialMailSender(void) {
    m_serial_port.set_format(8, BufferedSerial::None, 1);  // 8N1 format
}

// Function to convert inputs to SerialMail::Value array
std::vector<SerialMail::Value> SerialMailSender::convertToSerialMailValues(const std::vector<std::array<uint8_t, 3>>& inputs) {
    std::vector<SerialMail::Value> raw_input_bytes;
    raw_input_bytes.reserve(inputs.size()); // Reserve space for efficiency

    for (const auto& input : inputs) {
        // Create SerialMail::Value from each 3-byte array
        raw_input_bytes.emplace_back(SerialMail::Value(input[0], input[1], input[2]));
    }

    return raw_input_bytes;
}


// Serialize and send the SerialMail data
void SerialMailSender::sendMail(void) {

    // Access the shared queue
    SendingQueue& sending_queue = SendingQueue::getInstance();

    while(true){
        osEvent evt = sending_queue.mail_box.get();
		if (evt.status == osEventMail) {

            // Prepare the FlatBufferBuilder
            // FlatBufferBuilder should ideally be re-initialized inside the while loop 
            // for each iteration, especially if you are processing multiple messages. 
            // This is because the FlatBufferBuilder does not automatically clear its
            // internal buffer, and reusing it without clearing can lead to undefined behavior or memory issues.
            flatbuffers::FlatBufferBuilder builder(1024);

            // Retrieve the message from the mail box
            SendingQueue::mail_t *sending_mail = (SendingQueue::mail_t *)evt.value.p;

            // Create Flatbuffers vector of bytes
            std::vector<SerialMail::Value> raw_input_bytes = convertToSerialMailValues(sending_mail->inputs);
            auto inputs = builder.CreateVectorOfStructs(raw_input_bytes.data(), raw_input_bytes.size());

            // Create Flatbuffers float array
            std::vector<float> classification_values = sending_mail->classification;
            auto classification = builder.CreateVector(classification_values.data(), classification_values.size());

            // Channel and Classification active
            bool classification_active = sending_mail->classification_active;
            bool channel = sending_mail->channel;

            // Create the SerialMail object
            auto orc = CreateSerialMail(builder, inputs, classification, classification_active, channel);
            builder.Finish(orc);

            // Get the buffer pointer and size
            uint8_t* buf = builder.GetBufferPointer();
            uint32_t size = builder.GetSize();

            // Send the size (4 bytes)
            m_serial_port.write(reinterpret_cast<const char*>(&size), sizeof(size));

            // Send the FlatBuffers buffer
            m_serial_port.write(reinterpret_cast<const char*>(buf), size);
            
            printf("sent\n");
            // Free the allocated mail to avoid memory leaks
			// make mail box empty
			sending_queue.mail_box.free(sending_mail); 

        }
    }
}
