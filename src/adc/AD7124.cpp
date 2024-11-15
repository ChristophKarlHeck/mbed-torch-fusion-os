
#include <AD7124.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "BLE_Service/Service.h"

// Constructor definition: Takes a reference to an existing SPI object
// and parameters for Vref, Gain, and databits
AD7124::AD7124(uint8_t databits, float Vref, float Gain)
    : spi(PA_7, PA_6, PA_5), databits(databits), Vref(Vref), Gain(Gain) {
    // Set up SPI communication
    spi.format(8, 0);  // 8 bits per frame, SPI Mode 0
    spi.frequency(1000000); // Set SPI frequency to 1 MHz
    // SPI setup
    spi.format(8, 0);  // 8 bits per frame, SPI Mode 0 (CPOL=0, CPHA=0)
    spi.frequency(1000000); // Set the SPI frequency (1 MHz in this case)

    // Log the initialized values for debugging
    TRACE("AD7124 initialized with:\n");
    TRACE("  Databits: %d\n", databits);
    TRACE("  Vref: %.2fV\n", Vref);
    TRACE("  Gain: %d\n", Gain);
}

void AD7124::reset(void){
    INFO("Reset ADC\n");
    
    // Send 0xFF bytes to reset the ADC's registers to their default state
    for (int i = 0; i <= 8; i++){
        spi.write(0xFF);  // Write reset byte to ADC
    }
}

char AD7124::status(void){
    // Send read command to the status register of the AD7124
    spi.write(AD7124_R | AD7124_STATUS_REG);

    // Read the status register value from the ADC
    char status = spi.write(0x00);

    // Log the status value in hexadecimal and binary format for debugging
    TRACE("ADC status = 0x%X, "BYTE_TO_BINARY_PATTERN"\n", status, BYTE_TO_BINARY(status));

    return status;
}

void AD7124::channel_reg(char RW){
    // RW = 1 -> read, else write
    char channel_reg[2] = {0};

    // If the operation is read, fetch the channel register values
    if(RW == AD7124::read){
        // Send read command to the channel 1 map register
        spi.write(AD7124_R | AD7124_CH1_MAP_REG);
        
        TRACE("Channel register =");
        
        // Loop to read two bytes of channel register data
        for (int i = 0; i <= 1; i++){
            channel_reg[i] = spi.write(0x00); // Read byte from ADC
            TRACE(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(channel_reg[i])); // Print the binary format of the byte
        }
        
        TRACE("\n");

    } else {
        // If the operation is write, set the channel configurations
        
        // Configure Channel 0 if flag0 is true
        if(flag0 == true){
            spi.write(AD7124_CH0_MAP_REG); // Send command for Channel 0 register
            
            // Define the channel settings for Channel 0
            const uint16_t channel_settings = AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) |
                                               AD7124_CH_MAP_REG_AINP(0) | AD7124_CH_MAP_REG_AINM(1);
                                               
            // Split the 16-bit settings into two bytes for transmission
            char channel_reg_set_ch0[] = {channel_settings >> 8 & 0xFF, channel_settings & 0xFF}; 
            
            // Write the channel settings for Channel 0
            for (int i = 0; i <= 1; i++){
                spi.write(channel_reg_set_ch0[i]);
            }
        }

        // Configure Channel 1 if flag1 is true
        if(flag1 == true){
            spi.write(AD7124_CH1_MAP_REG); // Send command for Channel 1 register
            
            // Define the channel settings for Channel 1
            const uint16_t channel_settings1 = AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(1) |
                                               AD7124_CH_MAP_REG_AINP(2) | AD7124_CH_MAP_REG_AINM(3);
                                               
            // Split the 16-bit settings into two bytes for transmission
            char channel_reg_set_ch1[] = {channel_settings1 >> 8 & 0xFF, channel_settings1 & 0xFF}; 
            
            // Write the channel settings for Channel 1
            for (int i = 0; i <= 1; i++){
                spi.write(channel_reg_set_ch1[i]);
            }
        }
    }
}

void AD7124::config_reg(uint8_t address ,char RW){
    /* 
     * @brief Reads or writes the configuration register of the AD7124.
     * 
     * This function either reads from or writes to the configuration register 
     * of the AD7124 ADC. The operation is determined by the `RW` flag:
     * - If `RW == AD7124::read`, it reads the configuration register and 
     *   traces the values.
     * - If `RW == AD7124::write`, it writes predefined configuration settings 
     *   to the register.
     */
    
    char config_reg[2] = {0};
    
    if(RW == AD7124::read){
        // Perform a read operation
        spi.write(AD7124_R | address); // Send read command to the specified register
        TRACE("ADC conf = ");
        
        for (int i = 0; i <= 1; i++){
            config_reg[i] = spi.write(0x00); // Read the register value
            TRACE(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(config_reg[i]));
        }
        TRACE("\n");
    }
    else{
        // Perform a write operation
        spi.write(address); // Send address for the register to write to
        
        char my_config[] = {
            (AD7124_CFG_REG_BIPOLAR) >> 8, 
            AD7124_CFG_REG_AIN_BUFP | AD7124_CFG_REG_AINN_BUFM | AD7124_CFG_REG_REF_SEL(2) | AD7124_CFG_REG_PGA(2)
        };
        
        // Write the configuration data
        for (int i = 0; i <= 1; i++){
            spi.write(my_config[i]); // Write configuration bytes
        } 
    }
}

void AD7124::filter_reg(uint8_t filt, char RW){
    // Default Full Scale: 384, Post filter = 011
    char filter_reg[3] = {0};

    if(RW == AD7124::read){
        // Send the read command to the specified filter register
        spi.write(AD7124_R | filt);
        TRACE("Filter register =");
        
        // Read the filter register values and log them
        for (int i = 0; i<=2; i++){
            filter_reg[i] = spi.write(0x00);
            TRACE(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(filter_reg[i]));
        }
        TRACE("\n");
    }
    else {
        // Write a predefined filter setting to the filter register
        spi.write(filt);
        char filter_reg_set[]={0x00, 0x00, 0x00}; // Default values for filter configuration
        for (int i = 0; i<=2; i++){
            spi.write(filter_reg_set[i]);
        }
    }
}

void AD7124::ctrl_reg(char RW) {
    // Array to hold the control register values when reading
    char contr_reg[2] = {0};

    // Check if the operation is to read the control register
    if (RW == AD7124::read) {
        // Send the read command to the control register
        spi.write(AD7124_R | AD7124_ADC_CTRL_REG);
        
        // Log the read operation for debugging purposes
        TRACE("ADC contr_reg =");
        
        // Loop through the control register bytes and read their values
        for (int i = 0; i <= 1; i++) {
            // Read the control register values (byte by byte)
            contr_reg[i] = spi.write(0x00);  
            // Log the read byte in binary format for debugging
            TRACE(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(contr_reg[i]));  
        }
        
        // End the logging of the register read
        TRACE("\n");

    } else {  // If the operation is to write to the control register
        // Send the control register address for writing
        spi.write(AD7124_ADC_CTRL_REG);
        
        // Configure the control register settings:
        // - Data Status: Enabled
        // - Reference Enable: Enabled
        // - Continuous Read: Enabled
        // - Power Mode: Set to 0 (default)
        // - ADC Mode: Set to 0 (default)
        // - Clock Selection: Set to 0 (default)
        const uint16_t contr_reg_settings = AD7124_ADC_CTRL_REG_DATA_STATUS | 
                                             AD7124_ADC_CTRL_REG_REF_EN | 
                                             AD7124_ADC_CTRL_REG_CONT_READ | 
                                             AD7124_ADC_CTRL_REG_POWER_MODE(0) | 
                                             AD7124_ADC_CTRL_REG_MODE(0) | 
                                             AD7124_ADC_CTRL_REG_CLK_SEL(0);

        // Prepare the control register values to be written (split into two bytes)
        char contr_reg_set[] = {contr_reg_settings >> 8 & 0xFF, contr_reg_settings & 0xFF};
        
        // Write the control register values byte by byte
        for (int i = 0; i <= 1; i++) {
            spi.write(contr_reg_set[i]);  // Send each byte to the ADC
        }   
    }
}



void AD7124::init(bool f0, bool f1){
    flag0 = f0;
    flag1 = f1;
    sync = 1;
    cs=0;

    reset();
    status();

    channel_reg(read); //activate 2 channels
    channel_reg(write); //activate 2 channels
    channel_reg(read); //activate 2 channels

//flags for if you want to have channel 0, or 1, or both active
    if(flag0 == true){
        //config reg 0
        config_reg(AD7124_CFG0_REG, read);   // read  configuration register
        config_reg(AD7124_CFG0_REG, write);  // write configuration register
        config_reg(AD7124_CFG0_REG, read);   // proof writing by reading again
        //filter reg 0
        filter_reg(AD7124_FILT0_REG, read);  // same with filter register
        filter_reg(AD7124_FILT0_REG, write);
        filter_reg(AD7124_FILT0_REG, read);
    }

    if(flag1 == true){
        //config reg 1
        //AD7124::config_reg(AD7124_CFG1_REG, read);
        config_reg(AD7124_CFG1_REG, write);
        config_reg(AD7124_CFG1_REG, read);
        //filter reg 1
        //AD7124::filter_reg(AD7124_FILT1_REG, read);
        filter_reg(AD7124_FILT1_REG, write);
        filter_reg(AD7124_FILT1_REG, read);
    }
    //xAD7124::calibrate(1,1,0,0);

    ctrl_reg(read);     // same with control register
    ctrl_reg(write);
    ctrl_reg(read);
    //AD7124::calibrate(1,0,0,0);
}

float AD7124::get_analog_value(long measurement) {
    /* calculate the analog value from the measurement */
    float voltage = (float)measurement / databits - 1;
    voltage = voltage * Vref / Gain;
    voltage *= 1000;

    printf("analog %.3f\n", voltage); // Note: %.3f for float precision
    return voltage;
}

void AD7124::read_thread_multiple_valuev2(){
    const int data_length = 6; //bytes
    int data_counter = 0;
    while (true){
        uint8_t data[4] = {0, 0, 0, 0};
        // these flags are because want entries for both channel 0 and 1 CH1:(A0, A1) CH2:(A2, A3)
        // so only fill the buffer once there is data for both
        bool f0 = !flag0;  
        bool f1 = !flag1; 

        float voltage_from_measurement_channel_0 = 0.0;
        float voltage_from_measurement_channel_1 = 0.0;
        
        while(((f0 == false) || (f1 == false))){
            INFO("%d %d", f0,f1)
            wait_us(1);
            for (int i = 0; i<4; i++){
                // Sends 0x00 and simultaneously receives a byte from the SPI slave device.
                data[i] = spi.write(0x00);
            }

            if((data[3] == 0) && (f0 == false)){
                int measurement_channel_0 = (((long)data[0] << 16)|((long)data[1] << 8)|((long)data[2] << 0));
                f0 = true;
                voltage_from_measurement_channel_0 = AD7124::get_analog_value(measurement_channel_0);
                //data_counter+=3; 
            }
            //add to channel 1 data
            if((data[3] == 1) && (f1 == false)){
                f1 = true;
                int measurement_channel_1 = (((long)data[0] << 16)|((long)data[1] << 8)|((long)data[2] << 0));
                voltage_from_measurement_channel_1 = AD7124::get_analog_value(measurement_channel_1);
                //data_counter+=3;
            }

        }
        data_counter+=3;
        printf("\n%d\n", data_counter);
        if (data_counter >= data_length/2 && mail_box.empty()){
            mail_t *mail = mail_box.try_alloc();
            mail->voltage_channel_0 = voltage_from_measurement_channel_0;
            mail->voltage_channel_1 = voltage_from_measurement_channel_1;
            mail_box.put(mail); // must be freed after in ad7124.h set mailing box length
            data_counter=0;

            //raise(SIGUSR1); // Softwareinterrupt causes service.h to send data and delete afterwards.
        } else if (data_counter >= data_length/2){
            data_counter = 0;
        }

    }

}