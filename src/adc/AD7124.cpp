
#include <AD7124.h>
#include <cstdint>
#include <cstdio>
#include <cstring>

// Constructor with initializer list
AD7124::AD7124(float databits, float vref, float gain, int spi_frequency, int model_input_size, int downsampling_rate)
    : m_spi(PA_7, PA_6, PA_5), m_databits(databits), m_vref(vref), m_gain(gain), m_spi_frequency(spi_frequency),
      m_model_input_size(model_input_size), m_downsampling_rate(downsampling_rate), m_flag_0(false), m_flag_1(false), m_read(1), m_write(0) {

    // Set up SPI communication
    m_spi.format(8, 0);  // 8 bits per frame, SPI Mode 0 (CPOL=0, CPHA=0)
    m_spi.frequency(m_spi_frequency); // Set the SPI frequency to 1 MHz

}

void AD7124::reset(void){
    INFO("Reset ADC\n");
    
    // Send 0xFF bytes to reset the ADC's registers to their default state
    for (int i = 0; i < 9; i++){
        m_spi.write(0xFF);  // Write reset byte to ADC
    }
}

char AD7124::status(void){
    // Send read command to the status register of the AD7124
    m_spi.write(AD7124_R | AD7124_STATUS_REG);

    // Read the status register value from the ADC
    char status = m_spi.write(0x00);

    // Log the status value in hexadecimal and binary format for debugging
    TRACE("ADC status = 0x%X, " BYTE_TO_BINARY_PATTERN "\n", status, BYTE_TO_BINARY(status));

    return status;
}

void AD7124::channel_reg(char RW){
    // RW = 1 -> read, else write
    
    // If the operation is read, fetch the channel register values
    if(RW == m_read){

        // Send read command to the channel 1 map register
        m_spi.write(AD7124_R | AD7124_CH1_MAP_REG);
        
        TRACE("Channel register =");
        
        // Loop to read two bytes of channel register data
        for (int i = 0; i < 2; i++){
            // Read the control register values (byte by byte)
            // Log the index and the read byte in binary format for debuggings
            TRACE(" Index %d: " BYTE_TO_BINARY_PATTERN, i, BYTE_TO_BINARY(m_spi.write(0x00)));
        }
        
        TRACE("\n");

    } else {
        // If the operation is write, set the channel configurations
        
        // Configure Channel 0 if flag0 is true
        if(m_flag_0 == true){
            m_spi.write(AD7124_CH0_MAP_REG); // Send command for Channel 0 register
            
            // Define the channel settings for Channel 0
            const uint16_t channel_settings = AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) |
                                               AD7124_CH_MAP_REG_AINP(0) | AD7124_CH_MAP_REG_AINM(1);
                                               
            // Split the 16-bit settings into two bytes for transmission
            char channel_reg_set_ch0[] = {channel_settings >> 8 & 0xFF, channel_settings & 0xFF}; 
            
            // Write the channel settings for Channel 0
            for (int i = 0; i < 2; i++){
                m_spi.write(channel_reg_set_ch0[i]);
            }
        }

        // Configure Channel 1 if flag1 is true
        if(m_flag_1 == true){
            m_spi.write(AD7124_CH1_MAP_REG); // Send command for Channel 1 register
            
            // Define the channel settings for Channel 1
            const uint16_t channel_settings1 = AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(1) |
                                               AD7124_CH_MAP_REG_AINP(2) | AD7124_CH_MAP_REG_AINM(3);
                                               
            // Split the 16-bit settings into two bytes for transmission
            char channel_reg_set_ch1[] = {channel_settings1 >> 8 & 0xFF, channel_settings1 & 0xFF}; 
            
            // Write the channel settings for Channel 1
            for (int i = 0; i < 2; i++){
                m_spi.write(channel_reg_set_ch1[i]);
            }
        }
    }
}

void AD7124::config_reg(uint8_t address ,char RW){
    // RW = 1 -> read, else write
      
    if(RW == m_read){
        // Perform a read operation
        m_spi.write(AD7124_R | address); // Send read command to the specified register
        TRACE("ADC conf = ");

        for (int i = 0; i < 2; i++) {
            // Read the control register values (byte by byte)
            // Log the index and the read byte in binary format for debuggings
            TRACE(" Index %d: " BYTE_TO_BINARY_PATTERN, i, BYTE_TO_BINARY(m_spi.write(0x00)));
        }
        TRACE("\n");
    }
    else{
        // Perform a write operation
        m_spi.write(address); // Send address for the register to write to
        
        char my_config[] = {
            (AD7124_CFG_REG_BIPOLAR) >> 8, 
            AD7124_CFG_REG_AIN_BUFP | AD7124_CFG_REG_AINN_BUFM | AD7124_CFG_REG_REF_SEL(2) | AD7124_CFG_REG_PGA(2)
        };
        
        // Write the configuration data
        for (int i = 0; i < 2; i++){
            m_spi.write(my_config[i]); // Write configuration bytes
        } 
    }
}

void AD7124::filter_reg(uint8_t filt, char RW){
    // RW = 1 -> read, else write

    if(RW == m_read){
        // Send the read command to the specified filter register
        m_spi.write(AD7124_R | filt);
        TRACE("Filter register =");

        // Read and log the filter register values
        for (int i = 0; i < 3; i++) {
            // Read the control register values (byte by byte)
            // Log the index and the read byte in binary format for debuggings
            TRACE(" Index %d: " BYTE_TO_BINARY_PATTERN, i, BYTE_TO_BINARY(m_spi.write(0x00)));
        }

        TRACE("\n");
    }
    else {
        // Write a predefined filter setting to the filter register
        m_spi.write(filt);
        char filter_reg_set[]={0x00, 0x00, 0x00}; // Default values for filter configuration
        for (int i = 0; i < 3; i++){
            m_spi.write(filter_reg_set[i]);
        }
    }
}

void AD7124::ctrl_reg(char RW) {

    // Check if the operation is to read the control register
    if (RW == m_read) {
        // Send the read command to the control register
        m_spi.write(AD7124_R | AD7124_ADC_CTRL_REG);

        // Log the read operation for debugging purposes
        TRACE("ADC contr_reg =");

        // Loop through the control register bytes and read their values
        for (int i = 0; i < 2; i++) {
            // Read the control register values (byte by byte)
            // Log the index and the read byte in binary format for debuggings
            TRACE(" Index %d: " BYTE_TO_BINARY_PATTERN, i, BYTE_TO_BINARY(m_spi.write(0x00)));
        }

        // End the logging of the register read
        TRACE("\n");

    } else {  // If the operation is to write to the control register
        // Send the control register address for writing
        m_spi.write(AD7124_ADC_CTRL_REG);
        
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
        for (int i = 0; i < 2; i++) {
            m_spi.write(contr_reg_set[i]);  // Send each byte to the ADC
        }   
    }
}



void AD7124::init(bool f0, bool f1){
    m_flag_0 = f0;
    m_flag_1 = f1;

    reset();
    status();

    channel_reg(m_read); //activate 2 channels
    channel_reg(m_write); //activate 2 channels
    channel_reg(m_read); //activate 2 channels

//flags for if you want to have channel 0, or 1, or both active
    if(m_flag_0){
        //config reg 0
        config_reg(AD7124_CFG0_REG, m_read);   // read  configuration register
        config_reg(AD7124_CFG0_REG, m_write);  // write configuration register
        config_reg(AD7124_CFG0_REG, m_read);   // proof writing by reading again
        //filter reg 0
        filter_reg(AD7124_FILT0_REG, m_read);  // same with filter register
        filter_reg(AD7124_FILT0_REG, m_write);
        filter_reg(AD7124_FILT0_REG, m_read);
        printf("channelo 0 active\n");
    }

    if(m_flag_1){
        //config reg 1
        config_reg(AD7124_CFG1_REG, m_read);
        config_reg(AD7124_CFG1_REG, m_write);
        config_reg(AD7124_CFG1_REG, m_read);
        //filter reg 1
        filter_reg(AD7124_FILT1_REG, m_read);
        filter_reg(AD7124_FILT1_REG, m_write);
        filter_reg(AD7124_FILT1_REG, m_read);
        printf("channelo 1 active\n");
    }
    //xAD7124::calibrate(1,1,0,0);

    ctrl_reg(m_read);     // same with control register
    ctrl_reg(m_write);
    ctrl_reg(m_read);
    //AD7124::calibrate(1,0,0,0);
}

float AD7124::get_analog_value(long measurement) {
    /* calculate the analog value from the measurement */
    float voltage = (float)measurement / m_databits - 1;
    voltage = voltage * m_vref / m_gain;
    voltage *= 1000;

    //printf("analog %.3f\n", voltage); // Note: %.3f for float precision
    return voltage;
}

void AD7124::read_voltage_from_both_channels(void){
    while (true){

        std::vector<float> inputs(m_model_input_size);

        for(int i = 0; i < m_model_input_size; i++){
            uint8_t data[4] = {0, 0, 0, 0};
            for(int j = 0; j < 4; j++){
                // Sends 0x00 and simultaneously receives a byte from the SPI slave device.
                data[j] = m_spi.write(0x00);
            }
            
            // Specify the data source channel for the incoming data.
            // data[3] == 0 --> channel 0
            // data[3] == 1 --> channel 1

            int measurement = (((long)data[0] << 16)|((long)data[1] << 8)|((long)data[2] << 0));
            inputs[i] = get_analog_value(measurement);

            //wait_us(1); // Sampling rate: 1 MHZ
            thread_sleep_for(m_downsampling_rate); // ms
        }

        if (mail_box.empty()){
            mail_t *mail = mail_box.try_alloc();
            mail->inputs = inputs; // copy float vactor to avoid weird reference errors
            mail_box.put(mail); // must be freed after in ad7124.h set mailing box length
            //raise(SIGUSR1); // Softwareinterrupt causes service.h to send data and delete afterwards.
        }
    }
}

int AD7124::read_data_continous(void){

    printf("read_data_continous\n");
    BufferedSerial pc(PB_6, PB_7);
    uint8_t bufbuf[9];
    pc.set_baud(19200);
    pc.set_format(
            /* bits */ 8,
            /* parity */ BufferedSerial::Odd,
            /* stop bit */ 1
    );
    thread_sleep_for(10); // ms
    while(1){
        uint8_t data [4] = {0,0,0,0};
        bool f0 = !m_flag_0; //these flags are because want entries for both channel 0 and 1
        bool f1 = !m_flag_1; // so only fill the buffer once there is data for both

        printf("%d,%d\n", f0, f1);
        while((f0 == false) || (f1 == false)){

            for (int i = 0; i < 4; i++){
                data[i] = m_spi.write(0x00);
            }

            printf("%d, %d, %d\n", data[3], f0, f1);
            if((f0 == false)){
                bufbuf[4] = data[2];//measurement         & 0xFF;
                bufbuf[3] = data[1];//(measurement >>  8) & 0xFF;
                bufbuf[2] = data[0];//(measurement >> 16) & 0xFF; 
                //measurement1 =  (((long)bufbuf[2] << 16)|((long)bufbuf[3] << 8)|((long)bufbuf[4] << 0));
                f0 = true;
                //printf("1:");
                //AD7124::get_analog_value(measurement1);
            }
            //add to channel 1 data
            if((f1 == false)){
                bufbuf[7] = data[2];//measurement1         & 0xFF;
                bufbuf[6] = data[1];//(measurement1 >>  8) & 0xFF;
                bufbuf[5] = data[0];//(measurement1 >> 16) & 0xFF;
                f1 = true;
                //measurement2 =  (((long)bufbuf[5] << 16)|((long)bufbuf[6] << 8)|((long)bufbuf[7] << 0));
                //printf("2: ");
                //AD7124::get_analog_value(measurement2);
            }

        }
        bufbuf[0] = 80;      // need start values
        bufbuf[1] = 0x01;     //board number (#1)
        bufbuf[8] = 0x0A;     //end character
        pc.write(bufbuf, sizeof(bufbuf));

        printf("send\n");
    }
    return 1;
 
}