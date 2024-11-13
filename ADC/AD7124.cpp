
#include <AD7124.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "BLE_Service/Service.h"

bool flag0, flag1;

void AD7124::init(bool f0, bool f1){
    flag0 = f0;
    flag1 = f1;
    sync = 1;
    cs=0;

    AD7124::reset();
    AD7124::status();

    AD7124::channel_reg(read); //activate 2 channels
    AD7124::channel_reg(write); //activate 2 channels
    AD7124::channel_reg(read); //activate 2 channels

//flags for if you want to have channel 0, or 1, or both active
    if(flag0 == true){
        //config reg 0
        AD7124::config_reg(AD7124_CFG0_REG, read);   // read  configuration register
        AD7124::config_reg(AD7124_CFG0_REG, write);  // write configuration register
        AD7124::config_reg(AD7124_CFG0_REG, read);   // proof writing by reading again
        //filter reg 0
        AD7124::filter_reg(AD7124_FILT0_REG, read);  // same with filter register
        AD7124::filter_reg(AD7124_FILT0_REG, write);
        AD7124::filter_reg(AD7124_FILT0_REG, read);
    }

    if(flag1 == true){
        //config reg 1
        //AD7124::config_reg(AD7124_CFG1_REG, read);
        AD7124::config_reg(AD7124_CFG1_REG, write);
        AD7124::config_reg(AD7124_CFG1_REG, read);
        //filter reg 1
        //AD7124::filter_reg(AD7124_FILT1_REG, read);
        AD7124::filter_reg(AD7124_FILT1_REG, write);
        AD7124::filter_reg(AD7124_FILT1_REG, read);
    }
    //xAD7124::calibrate(1,1,0,0);

    AD7124::ctrl_reg(read);     // same with control register
    AD7124::ctrl_reg(write);
    AD7124::ctrl_reg(read);
    //AD7124::calibrate(1,0,0,0);
}

double AD7124::get_analog_value(long measurement){
    /* calculate the analog value from the measurement */
    double voltage = (double)measurement/databits -1;
    voltage = voltage * Vref/ Gain;
    voltage *= 1000;
    printf("analog %.3F\n", voltage);
    return voltage;
}

void AD7124::reset(){
    /* reset the ADC */
    INFO("Reset ADC\n");
    for (int i = 0; i<=8; i++){
        spi.write(0xFF);
    }
}

char AD7124::status(){
    /* read the status register */
    spi.write(AD7124_R | AD7124_STATUS_REG);
    char status = spi.write(0x00); 
    TRACE("ADC status = 0x%X, "BYTE_TO_BINARY_PATTERN"\n", status,BYTE_TO_BINARY(status));
    return status;
}

int AD7124::error_reg(){
    /* read the error register */
    int sum = 0;
    spi.write(AD7124_R | AD7124_ERR_REG); // read channel 1
    char error_reg[3] = {0};
    TRACE("Error register =");
    for (int i = 0; i<=2; i++){
        error_reg[i] = spi.write(0x00);
        TRACE(" "BYTE_TO_BINARY_PATTERN,BYTE_TO_BINARY(error_reg[i]));
        sum += error_reg[i];
    }
    TRACE("\n");  
    return sum;
}


void AD7124::read_ID(){
    // acces ID
    spi.write(AD7124_R | AD7124_ID_REG);
    //read ID by sending dumm byte 
    int ID = spi.write(0x00);
    printf("ADC ID = 0x%X, "BYTE_TO_BINARY_PATTERN"\n", ID,BYTE_TO_BINARY(ID));
    // Deselect the device
}


void AD7124::calibrate(bool internal_zero_scale, bool internal_full_scale, bool system_zero_scale, bool system_full_scale){
    // calibrate the ADC, Full power mode, ref enable
    TRACE("Calibration\n");

    if(system_full_scale){
        spi.write(AD7124_ADC_CTRL_REG); // 
        char control_reg[2] = {AD7124_ADC_CTRL_REG_REF_EN>>8,
        AD7124_ADC_CTRL_REG_POWER_MODE(0) | AD7124_ADC_CTRL_REG_MODE(0x08)}; 
        TRACE("SYSTEM FULL SCALE\n");
        for (int i = 0; i<=1; i++){
            spi.write(control_reg[i]);
        }
        /* wait for the calibration to finish */
        char status = AD7124::status();
        while(status){
            status = AD7124::status();
        }
        TRACE("SYSTEM FULL SCALE done\n");
    }


    if(system_zero_scale){
        spi.write(AD7124_ADC_CTRL_REG);
        char control_reg[2] = {AD7124_ADC_CTRL_REG_REF_EN>>8,
        AD7124_ADC_CTRL_REG_POWER_MODE(0) | AD7124_ADC_CTRL_REG_MODE(0x07)};
        TRACE("SYSTEM ZERO SCALE\n");
        for (int i = 0; i<=1; i++){
            spi.write(control_reg[i]);
        }
        /* wait for the calibration to finish */
        char status = AD7124::status();
        while(status){
            status = AD7124::status();
        }
        TRACE("SYSTEM ZERO SCALE done\n");
    }


    if (internal_full_scale){
        spi.write(AD7124_ADC_CTRL_REG); // 
        char control_reg[2] = {AD7124_ADC_CTRL_REG_REF_EN>>8,
        AD7124_ADC_CTRL_REG_POWER_MODE(0) | AD7124_ADC_CTRL_REG_MODE(0x06)}; 
        TRACE("INTERNAL FULL SCALE\n");
        for (int i = 0; i<=1; i++){
            spi.write(control_reg[i]);
        }
        /* wait for the calibration to finish */
        char status = AD7124::status();
        while(status){
        status = AD7124::status();
        }
        TRACE("INTERNAL FULL SCALE done\n");
    }

    if (internal_zero_scale){
        spi.write(AD7124_ADC_CTRL_REG); // 
        char control_reg[2] = {AD7124_ADC_CTRL_REG_REF_EN>>8,
        AD7124_ADC_CTRL_REG_POWER_MODE(0) | AD7124_ADC_CTRL_REG_MODE(0x05)}; 
        TRACE("INTERNAL ZERO SCALE\n");
        for (int i = 0; i<=1; i++){
            spi.write(control_reg[i]);
        }
        /* wait for the calibration to finish */
        char status = AD7124::status();
        while(status){
            status = AD7124::status();
        }
        TRACE("INTERNAL ZERO SCALE done\n");
    }
    AD7124::ctrl_reg(write);
}


void AD7124::config_reg(uint8_t address ,char RW){
    /* read/ write the configuration register */
    
    char config_reg[2] = {0};
    if(RW == AD7124::read){
        spi.write(AD7124_R | address);
        TRACE("ADC conf = ");
        for (int i = 0; i<=1; i++){
            config_reg[i] = spi.write(0x00);
            TRACE(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(config_reg[i]));
        }
        TRACE("\n");
    }
    else{
        spi.write(address);
        char my_config[]={(AD7124_CFG_REG_BIPOLAR ) >>8 ,
                          AD7124_CFG_REG_AIN_BUFP | AD7124_CFG_REG_AINN_BUFM | AD7124_CFG_REG_REF_SEL(2) |AD7124_CFG_REG_PGA(2)};
        //original 0x08, 0x71
        for (int i = 0; i<=1; i++){
            spi.write(my_config[i]);
        } 
    }
}


void AD7124::ctrl_reg(char RW){
    /* read/write the control register */
    char contr_reg[2] = {0};

    if(RW == AD7124::read){
        spi.write(AD7124_R | AD7124_ADC_CTRL_REG);
        TRACE("ADC contr_reg =");
        for (int i = 0; i<=1; i++){
            contr_reg[i] = spi.write(0x00);
            TRACE(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(contr_reg[i]));
        }
        TRACE("\n");
    } else {
        spi.write(AD7124_ADC_CTRL_REG);
        const uint16_t contr_reg_settings = AD7124_ADC_CTRL_REG_DATA_STATUS | AD7124_ADC_CTRL_REG_REF_EN | AD7124_ADC_CTRL_REG_CONT_READ| 
                                        AD7124_ADC_CTRL_REG_POWER_MODE(0) | AD7124_ADC_CTRL_REG_MODE(0)|AD7124_ADC_CTRL_REG_CLK_SEL(0);
        char contr_reg_set[]={contr_reg_settings>>8 & 0xFF, contr_reg_settings & 0xFF};

        for (int i = 0; i<=1; i++){
            spi.write(contr_reg_set[i]);
        }   
    }
}


/* channel_reg
 * Sets up the channel registers.
 * Can setup channel 0, or 1, or both depending on flags
 */
void AD7124::channel_reg(char RW){
    //RW=1 -> read else write
    char channel_reg[2] = {0};

    if(RW == AD7124::read){
        spi.write(AD7124_R | AD7124_CH1_MAP_REG);
        TRACE("Channel register =");
        for (int i = 0; i<=1; i++){
            channel_reg[i] = spi.write(0x00);
            TRACE(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(channel_reg[i]));
        }
        TRACE("\n");

    } else {
        // SET CHANNEL 0
        if(flag0 == true){
            spi.write(AD7124_CH0_MAP_REG);
            const uint16_t channel_settings = AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(0) |
                                                 AD7124_CH_MAP_REG_AINP(0) |AD7124_CH_MAP_REG_AINM(1);
            char channel_reg_set_ch0[]={channel_settings>>8 & 0xFF, channel_settings & 0xFF}; //channel 0 and 1 (0x80, 0x01)
                //0x80 for setup 0
            for (int i = 0; i<=1; i++){
                spi.write(channel_reg_set_ch0[i]);
            }
        }
        // SET CHANNEL 1
        if(flag1 == true){
            spi.write(AD7124_CH1_MAP_REG);
            //register bytes set like this -> 10 00 00 (00 - 01 0)(0 00 11)
            //channel 1 - pins (2) and (3)
            //x80 is 1st byte, x43 is 2nd byte (for setting the AIN)
            const uint16_t channel_settings1 = AD7124_CH_MAP_REG_CH_ENABLE | AD7124_CH_MAP_REG_SETUP(1) |
                                        AD7124_CH_MAP_REG_AINP(2) |AD7124_CH_MAP_REG_AINM(3);

            char channel_reg_set_ch1[]={channel_settings1>>8 & 0xFF, channel_settings1 & 0xFF}; //channel 2 and 3 (0x90, 0x43)
                //0x90 for setup 1
            for (int i = 0; i<=1; i++){
                spi.write(channel_reg_set_ch1[i]);
            }
        }
    }
}



void AD7124::filter_reg(uint8_t filt, char RW){
    //default FS: 384, Post filter = 011
    char filter_reg[3] = {0};

    if(RW == AD7124::read){
        spi.write(AD7124_R | filt);
        TRACE("Filter register =");
        for (int i = 0; i<=2; i++){
            filter_reg[i] = spi.write(0x00);
            TRACE(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(filter_reg[i]));
        }
        TRACE("\n");
    }
    // else {
    //     if(filt == AD7124_FILT0_REG){
    //         spi.write(filt);
    //         //char contr_reg_set[]={0x00,0x08};
    //         //char filter_reg_set[]={AD7124_FILT_REG_FILTER(4)>>16,0x00,0x40};
    //         char filter_reg_set[]={0x20,0x00,0x40}; //0x00,0x12,0xC0 for testing
    //         for (int i = 0; i<=2; i++){
    //             spi.write(filter_reg_set[i]);
    //         }
    //         printf("reach 0");
    //     }
    // ^ this was for testing different filters
    else{
        spi.write(filt);
        //char contr_reg_set[]={0x00,0x08};
        //char filter_reg_set[]={AD7124_FILT_REG_FILTER(4)>>16,0x00,0x40};
        char filter_reg_set[]={0x00, 0x00, 0x00}; //0x00,0x12,0xC0 for testing
        for (int i = 0; i<=2; i++){
            spi.write(filter_reg_set[i]);
        }
    }
}



long AD7124::read_data(void){
    spi.write(AD7124_R | AD7124_DATA_REG);
    char data [3] = {0};
    for (int i = 0; i<=2; i++){
        data[i] = spi.write(0x00);
       // printf(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(data[i]));
    }
    //printf("\n");
    long measurement =  (((long)data[0] << 16)|((long)data[1] << 8)|((long)data[2] << 0));
    //printf("measurement %ld 0x%lX, %d, %d, %d\n", measurement, measurement, data[0], data[1], data[2]);
    AD7124::get_analog_value(measurement);
    return measurement;
}

uint8_t * AD7124::read_data_byte(){
    //LD =!LD;
    spi.write(AD7124_R | AD7124_DATA_REG);
    static uint8_t data [3] = {0};
    char data1 [4] = {0};
    for (int i = 0; i<=2; i++){
        data[i] = spi.write(0x00);
       // printf(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(data[i]));
    }
    //printf("\n");
    //char bufbuf[] = "In read data byte\n\r";
    //FT232.write(bufbuf, sizeof(bufbuf));

    //FT232.write(data1, sizeof(data1));
    long measurement =  (((long)data[0] << 16)|((long)data[1] << 8)|((long)data[2] << 0));
    //printf("measurement %ld 0x%lX, %d, %d, %d\n", measurement, measurement, data[0], data[1], data[2]);

    //AD7124::get_analog_value(true, measurement);
    //char bufbuf1[] = "\n\r";
    //FT232.write(bufbuf1, sizeof(bufbuf1));

    //long measurement =  (((long)data[0] << 16)|((long)data[1] << 8)|((long)data[2] << 0));
    //printf("measurement %ld 0x%lX, %d, %d, %d\n", measurement, measurement, data[0], data[1], data[2]);

    AD7124::get_analog_value(measurement);
    return data;
}

void AD7124::read_thread_single_value(){
    while (1){
        static uint8_t data [3] = {0};
        //INFO("IM IN")
        double volt = 0;
        int avg_num = 100;
        long measurement = 0;
        for ( int avg_counter = 0; avg_counter<avg_num; avg_counter++){
            if (AD7124::status()>>7 == 0) {
                measurement += AD7124::read_data(); // read data register
                volt += AD7124::get_analog_value(measurement/(avg_counter+1));
                //INFO("%d %f \n",avg_counter, volt);
            }else{
                avg_counter--;
            }
        }
        
        volt = volt/avg_num;
        INFO("%f ", volt)
        measurement = measurement/avg_num;
        
        mail_t *mail = mail_box.try_alloc();
        mail->voltage = volt;
        mail->ready = true;
        mail->raw_measurement = measurement;
        mail_box.put(mail); // must be freed after in ad7124.h set mailing box length
        //mail_box.free(mail);
    }
}


void AD7124::read_thread_multiple_value(){
    const int data_length = 99;
    static uint8_t data [data_length] = {0};
    static uint8_t * data_point;
    while (1){
        //wait_us(2500);
        INFO("IM IN")
        if (AD7124::status()>>7 == 0){
            data_point = AD7124::read_data_byte();
            for (int i = 0; i<3; i ++){
                int idx = data_counter + i;
                //#printf("%d\n",idx);
                data[idx] = data_point[i] ;
            }
            data_counter+=3;
        }
        //uint8_t *ptr = data;
        if (data_counter >= data_length && mail_box.empty()){
            //printf("----mailing---%d --- %d ---\n", data[0],data[239]);
            mail_t *mail = mail_box.try_alloc();
            mail->voltage = 1;
            mail->ready = true;
            mail->raw_measurement = 1;
            mail->raw_measurement_array = data;
            
            //memcpy(mail->raw_measurement_array, data, sizeof(mail->raw_measurement_array));
            //printf("----test---%d --- %d ---\n", mail->raw_measurement_array[0],mail->raw_measurement_array[239]);

            //mail->raw_measurement_array = data;
            mail_box.put(mail); // must be freed after in ad7124.h set mailing box length
            data_counter=0;
            //call the interrupt here


        } else if (data_counter >= data_length){
            data_counter = 0;
        }

        
        /*
        volt = volt/avg_num;
        INFO("%f ", volt)
        measurement = measurement/avg_num;
        
        mail_t *mail = mail_box.try_alloc();
        mail->voltage = volt;
        mail->ready = true;
        mail->raw_measurement = measurement;
        mail_box.put(mail); // must be freed after in ad7124.h set mailing box length
        //mail_box.free(mail);
        */
    }
}

void AD7124::read_thread_multiple_valuev2(){
    /* stopppeed here somewhere */
    //printf("thread");
    const int data_length = 6; //bytes
    static uint8_t data_tosend [data_length] = {0};
    static uint8_t * data_point;
    int data_counter = 0;
    int count1 = 0;
    int count2 = 0;
    while (1){
        //printf("im in the loop");
        uint8_t *cur_data;
        static uint8_t data [4] = {0};
        bool f0 = !flag0; //these flags are because want entries for both channel 0 and 1 CH1:(A0, A1) CH2:(A2, A3) 
        bool f1 = !flag1; // so only fill the buffer once there is data for both
        long measurement1, measurement2 = 0;
        while(((f0 == false) || (f1 == false))){
            INFO("%d %d", f0,f1)
            while (drdy == 0) {  // wait if chip still ready from last cycle
                //wait_us(1); 
                }
            while (drdy == 1) {  // wait for chip to be ready
                //wait_us(1);
            }
            wait_us(1);
            for (int i = 0; i<=3; i++){
                data[i] = spi.write(0x00);
                //printf(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(data[i]));
            }
            //printf("\n");

            measurement1, measurement2 =0;
            if (data[3] == 0) {
                //printf("1\n");
                count1 +=1;
            } else if (data[3] == 1 ) {
                //printf("2\n");
            count2 += 1;
            }
            
            
            if((data[3] == 0) && (f0 == false)){
                //printf("1\n");//  %d\n");//,  data_counter + 2);
                data_tosend[data_counter + 2] = data[2];//measurement         & 0xFF;
                data_tosend[data_counter + 1] = data[1];//(measurement >>  8) & 0xFF;
                data_tosend[data_counter + 0] = data[0];//(measurement >> 16) & 0xFF; 
                measurement1 =  (((long)data[0] << 16)|((long)data[1] << 8)|((long)data[2] << 0));
                f0 = true;
                //printf("1: %ld   ", measurement1);
                //AD7124::get_analog_value(measurement1);
                //data_counter+=3; 
            }
            //add to channel 1 data
            if((data[3] == 1) && (f1 == false)){
                //printf("2\n");//  %d", data_length/2 + data_counter + 2);
                data_tosend[data_counter + data_length/2 + 2] = data[2];//measurement1         & 0xFF;
                data_tosend[data_counter + data_length/2 + 1] = data[1];//(measurement1 >>  8) & 0xFF;
                data_tosend[data_counter + data_length/2 + 0] = data[0];//(measurement1 >> 16) & 0xFF;
                f1 = true;
                measurement2 =  (((long)data[0] << 16)|((long)data[1] << 8)|((long)data[2] << 0));
                //printf("2: %ld ", measurement2);
                //AD7124::get_analog_value(measurement2);
                //data_counter+=3;
            }

        }
        data_counter+=3;
        //printf("\n%d\n", data_counter);
        //uint8_t *ptr = data;
        if (data_counter >= data_length/2 && mail_box.empty()){
            //printf("sended %d %d \n", count1, count2);
            count1 = 0;
            count2 = 0;
            //printf("----mailing---%d --- %d ---\n", data[0],data[239]);
            mail_t *mail = mail_box.try_alloc();
            mail->voltage = 1;
            mail->ready = true;
            mail->raw_measurement = 1;
            mail->raw_measurement_array = data_tosend;
            
            //memcpy(mail->raw_measurement_array, data, sizeof(mail->raw_measurement_array));
            //printf("----test---%d --- %d ---\n", mail->raw_measurement_array[0],mail->raw_measurement_array[239]);

            //mail->raw_measurement_array = data;
            mail_box.put(mail); // must be freed after in ad7124.h set mailing box length
            data_counter=0;
            //printf("\nmail PUT\n");
            raise(SIGUSR1); // Softwareinterrupt causes service.h to send data and delete afterwards.
        } else if (data_counter >= data_length/2){
            data_counter = 0;
        }

    }
}

void AD7124::read_thread_multiple_valuev3(){
    /* stopppeed here somewhere */
    const int data_length = 6;//102;
    static uint8_t data_tosend [data_length] = {0};
    static uint8_t * data_point;
    int data_counter = 0;
    int count1 = 0;
    int count2 = 0;
    while (1){
        printf("hsfgsergsi");
        uint8_t *cur_data;
        static uint8_t data [4] = {0};
        bool f0 = !flag0; //these flags are because want entries for both channel 0 and 1
        bool f1 = !flag1; // so only fill the buffer once there is data for both
        long measurement1, measurement2 = 0;

        while(((f0 == false) || (f1 == false)) && mail_box.empty()){
            while (drdy == 0) {  // wait if chip still ready from last cycle
                wait_us(1); //<---- before 1000
                }
            while (drdy == 1) {  // wait for chip to be ready
                wait_us(1);
            }
            for (int i = 0; i<=3; i++){
                data[i] = spi.write(0x00);
                //printf(" "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(data[i]));
            }
            //printf("\n");

            measurement1, measurement2 =0;
            if (data[3] == 0) {
                count1 +=1;
            } else if (data[3] == 1 ) {
            count2 += 1;
            }
            printf("%d %d", count1, count2);
            if((data[3] == 0) && (f0 == false)){
                printf("1 %d",data_counter + 2);
                data_tosend[data_counter + 2] = data[2];//measurement         & 0xFF;
                data_tosend[data_counter + 1] = data[1];//(measurement >>  8) & 0xFF;
                data_tosend[data_counter + 0] = data[0];//(measurement >> 16) & 0xFF; 
                measurement1 =  (((long)data[0] << 16)|((long)data[1] << 8)|((long)data[2] << 0));
                f0 = true;
                //printf("1: %ld   ", measurement1);
                //A/D7124::get_analog_value(measurement1);
                //data_counter+=3; 
            }
            //add to channel 1 data
            if((data[3] == 1) && (f1 == false)){
                printf("2  %d", data_length/2 + data_counter + 2);
                data_tosend[data_length/2 + data_counter + 2] = data[2];//measurement1         & 0xFF;
                data_tosend[data_length/2 + data_counter + 1] = data[1];//(measurement1 >>  8) & 0xFF;
                data_tosend[data_length/2 + data_counter + 0] = data[0];//(measurement1 >> 16) & 0xFF;
                f1 = true;
                measurement2 =  (((long)data[0] << 16)|((long)data[1] << 8)|((long)data[2] << 0));
                //printf("2: %ld ", measurement2);
                //AD7124::get_analog_value(measurement2);
                //data_counter+=3;
            }

        }
        data_counter+=3;
        //printf("\n%d\n", data_counter);
        //uint8_t *ptr = data;
        if (data_counter >= data_length/2 && mail_box.empty()){
            //printf("----mailing---%d --- %d ---\n", data[0],data[239]);
            mail_t *mail = mail_box.try_alloc();
            mail->voltage = 1;
            mail->ready = true;
            mail->raw_measurement = 1;
            mail->raw_measurement_array = data_tosend;
            
            //memcpy(mail->raw_measurement_array, data, sizeof(mail->raw_measurement_array));
            //printf("----test---%d --- %d ---\n", mail->raw_measurement_array[0],mail->raw_measurement_array[239]);

            //mail->raw_measurement_array = data;
            mail_box.put(mail); // must be freed after in ad7124.h set mailing box length
            data_counter=0;
            //printf("\nmail PUT\n");
        } else if (data_counter >= data_length/2){
            data_counter = 0;
        }

    }
}