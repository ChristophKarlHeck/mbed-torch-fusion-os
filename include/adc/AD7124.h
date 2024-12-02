#ifndef AD7124_H_
#define AD7124_H_

#include "PinNames.h"
#include "adc/ad7124-defs.h"
#include "mbed.h"
#include "utils/logger.h"
#include <cstdint>
#include <cstdio>
#include <mutex>

class AD7124 : private mbed::NonCopyable<AD7124> {
public:
    // Static method to get the singleton instance
    static AD7124& getInstance(int spi_frequency) {
        static AD7124 instance(spi_frequency); // Thread-safe in C++11 and later
        return instance;
    }

    // Delete copy constructor and assignment operator to ensure singleton
    AD7124(const AD7124&) = delete;
    AD7124& operator=(const AD7124&) = delete;

    // Public methods
    void init(bool f0, bool f1);
    void read_voltage_from_both_channels(unsigned int downsampling_rate, unsigned int model_input_size);

private:
    // Private constructor
    AD7124(int spi_frequency);

    // Member variables
    SPI m_spi;                      // SPI object for communication with the AD7124
    int m_spi_frequency;            // SPI Frequency
    bool m_flag_0;                  // Flags for channel configuration
    bool m_flag_1;
    char m_read;                    // Read operation indicator
    char m_write;                   // Write operation indicator

    // Private helper methods
    void reset(void);
    char status(void);
    void channel_reg(char RW);
    void config_reg(uint8_t address, char RW);
    void filter_reg(uint8_t filt, char RW);
    void ctrl_reg(char RW);

    void send_data_to_main_thread(
        std::vector<std::array<uint8_t,3>> byte_inputs_channel_0,
        std::vector<std::array<uint8_t,3>> byte_inputs_channel_1,
        unsigned int model_input_size);
};

#endif /* AD7124_H_ */
