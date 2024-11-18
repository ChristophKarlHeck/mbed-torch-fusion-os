
#ifndef ADC_PROCESS_H_
#define ADC_PROCESS_H_

#include "DigitalOut.h"
#include "PinNames.h"
#include "mbed.h"
#include "hal/include/hal/spi_api.h"
#include "mstd_iterator"
#include <cstdint>
#include <cstdio>
#include <ad7124-defs.h>
#include <utils.h>
#include <logger.h>

/**
 * @brief SPI interface for communication with the AD7124 ADC.
 * 
 * This `SPI` object establishes a communication interface between the microcontroller
 * and the AD7124 ADC. It uses the following pins for data transmission:
 * - **PA_7**: MOSI (Master Out Slave In) - Sends data from the microcontroller to the AD7124.
 * - **PA_6**: MISO (Master In Slave Out) - Receives data from the AD7124 to the microcontroller.
 * - **PA_5**: SCLK (Serial Clock) - Provides the clock signal for SPI communication.
 * 
 * This interface facilitates the transmission of commands, configuration settings, 
 * and data to/from the AD7124, enabling communication between the two devices.
 * 
 * @note The pin assignments (PA_7, PA_6, and PA_5) are specific to the microcontroller 
 * and must be correctly wired to the corresponding SPI pins on the AD7124 for proper operation.
 * Ensure that these connections match the microcontroller's SPI peripheral configuration.
 */
//SPI spi(PA_7, PA_6, PA_5); // mosi, miso, sclk

/* 
 * Mail: 
 * 
 * This Mail structure is used for Inter-Thread Communication between the 
 * reading data thread and the main thread. It facilitates the exchange of 
 * data between threads in a safe and efficient manner.
 */
typedef struct {
    std::vector<float> inputs; /* Vector of downsampled analog values*/
    //float input;
} mail_t;

/**
 * @class AD7124
 * @brief Class for interacting with the AD7124 ADC.
 * 
 * This class provides methods for controlling and reading from the AD7124
 * analog-to-digital converter (ADC). It supports configuring channels, 
 * reading data, resetting the ADC, and managing the ADC's status.
 */
class AD7124: private mbed::NonCopyable<AD7124>{
    public:

        Mail<mail_t, 4> mail_box;

        // Constructor with parameters for databits, Vref, and Gain
        AD7124(float databits, float vref, float gain);

        void init(bool f0, bool f1);
        void read_voltage_from_both_channels(int number_of_input_values);
    
    private:
        SPI m_spi;                  // SPI object for communication with the AD7124
        float m_databits;           // Data bits used in measurement calculations
        float m_vref;	            // Reference voltage
        float m_gain;               // Gain factor
        bool m_flag_0;              // Flags for channel configuration
        bool m_flag_1;
        char m_read;                // Read operation indicator
        char m_write;               // Write operation indicator
        
        /**
         * @brief Resets the AD7124 ADC to its default state.
         *
         * This function sends a series of `0xFF` bytes over the SPI interface to 
         * reset the ADC's registers to their default state. This is typically used 
         * to return the ADC to a known state before performing further configurations.
         * 
         * @note This function uses SPI communication to send the reset command to 
         * the ADC. The ADC will be reset to its default configuration, and any 
         * previous settings or configurations will be cleared.
         */
        void reset(void);

        /**
         * @brief Reads the status register of the AD7124 ADC.
         * 
         * This function sends a read command to the status register of the AD7124 and 
         * retrieves the current status value. It then logs the status in both hexadecimal 
         * and binary formats for debugging purposes.
         * 
         * @return char The status value read from the ADC's status register.
         * 
         * @note This function sends the appropriate read command over SPI, waits for the 
         * response, and then returns the status value. The status is logged using the 
         * `TRACE` macro for debugging.
         */
        char status(void);

        /**
         * @brief Sets up the channel registers of the AD7124.
         * 
         * This function configures the channel registers of the AD7124 based on the 
         * provided read/write flag. It can configure channel 0, channel 1, or both 
         * depending on the flags `flag0` and `flag1`. If the operation is a read, 
         * it retrieves the current channel configuration. If the operation is a write, 
         * it writes the appropriate configuration to the channel registers.
         * 
         * @param RW A flag indicating whether the operation is a read (`1`) or write (`0`).
         * 
         * @note 
         * - If `RW` is `1`, it performs a read operation, retrieving the channel register values.
         * - If `RW` is `0`, it performs a write operation to configure channel 0 and/or channel 1.
         * - Channel 0 and Channel 1 configurations are controlled by the flags `flag0` and `flag1`.
         * 
         * @details
         * The function performs the following:
         * - For a read operation (`RW == AD7124::read`), it reads the channel register values and traces them.
         * - For a write operation (`RW == AD7124::write`), it configures channel 0 and/or channel 1 based on the flags:
         *   - Channel 0 is configured if `flag0` is `true`.
         *   - Channel 1 is configured if `flag1` is `true`.
         */
        void channel_reg(char RW);

        /**
         * @brief Configures or reads the configuration register of the AD7124.
         *
         * This function is used to either read or write the configuration register 
         * of the AD7124 ADC. If the operation is a read, it retrieves the configuration 
         * register values and traces them. If the operation is a write, it writes 
         * predefined configuration settings to the register.
         * 
         * @param address The register address to be accessed (configuration register).
         * @param RW A flag indicating whether the operation is a read (`1`) or write (`0`).
         * 
         * @note 
         * - If `RW` is `AD7124::read`, the function reads the configuration register and prints the value in binary.
         * - If `RW` is `AD7124::write`, the function writes predefined configuration values to the register.
         * 
         * @details
         * For a read operation (`RW == AD7124::read`), the function sends a read command to the specified address 
         * and then reads the configuration register, displaying the result in binary format. 
         * For a write operation (`RW == AD7124::write`), it sends a configuration byte sequence to set 
         * the configuration of the AD7124 ADC, including settings for bipolar input, reference selection, 
         * and programmable gain amplifier (PGA).
         */
        void config_reg (uint8_t adress, char RW);

        /**
         * @brief Reads or writes to the filter register of the AD7124 ADC.
         * 
         * This function can either read the current configuration of the filter register 
         * or write a new filter configuration to the ADC based on the provided flag. 
         * The function communicates with the AD7124 using SPI and performs the operation 
         * according to the `RW` flag. 
         * 
         * @param filt The filter register address to be accessed.
         * @param RW A flag indicating whether to read (`1`) or write (`0`) to the filter register.
         * 
         * @note 
         * - If `RW` is `1`, the function reads the filter register values and logs them.
         * - If `RW` is `0`, it writes a predefined filter setting to the specified filter register.
         * 
         * @details 
         * The function communicates with the AD7124's filter register over SPI. For reading, 
         * it retrieves and logs the filter register values. For writing, it writes a fixed filter 
         * configuration to the ADC.
         */
        void filter_reg(uint8_t filt, char RW);

        /**
         * @brief Reads or writes the control register of the AD7124.
         *
         * This function allows reading from or writing to the control register of the AD7124 ADC. 
         * It interacts with the ADC's control register to configure various parameters such as 
         * power mode, reference enable, continuous read, and clock selection. The behavior of the 
         * function depends on the provided flag `RW`, where:
         * - If `RW` is set to `AD7124::read`, it performs a read operation.
         * - If `RW` is set to `AD7124::write`, it writes the appropriate configuration to the control register.
         * 
         * @param RW A flag indicating whether to read (`AD7124::read`) or write (`AD7124::write`) to the control register.
         * 
         * @note The write operation configures several control bits:
         * - Data Status
         * - Reference Enable
         * - Continuous Read
         * - Power Mode
         * - ADC Mode
         * - Clock Selection
         */
        void ctrl_reg(char RW);

        /**
         * @brief Calculates the analog voltage corresponding to the given digital measurement.
         * 
         * This function converts a digital measurement value (from the ADC) into its corresponding 
         * analog voltage using the reference voltage (Vref), the gain factor, and the ADC's data resolution.
         * The result is returned in millivolts (mV) for easier interpretation of the signal.
         * 
         * @param measurement The digital measurement value from the ADC.
         * 
         * @return The corresponding analog voltage in millivolts (mV).
         * 
         * @note The calculation assumes the measurement is from an ADC with a known resolution (databits), 
         *       reference voltage (Vref), and gain factor (Gain).
         */
        float get_analog_value(long measurement);


};
#endif /* GATT_SERVER_EXAMPLE_BLE_PROCESS_H_ */
