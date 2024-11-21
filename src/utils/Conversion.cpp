#include "Conversion.h" // Include the header for the function declaration

// Function definition
std::vector<float> get_analog_inputs(std::vector<std::array<uint8_t,3>> byte_inputs, float databits, float vref, float gain) {
    /* calculate the analog value from the measurement */

    std::vector<float> inputs;

    for(unsigned int i = 0; i < byte_inputs.size(); i++){
        int measurement =  (((int)byte_inputs[i][0] << 16)|(int)byte_inputs[i][1] << 8)|((int)byte_inputs[i][2] << 0);
        float voltage = (float)measurement / databits - 1;
        voltage = voltage * vref / gain;
        voltage *= 1000;
        inputs.push_back(voltage);
    }

    // for (size_t i = 0; i < inputs.size(); ++i) {
    //     printf("inputs[%zu] = %.3f\n", i, inputs[i]);  // %.3f prints 3 decimal places
    // }

    return inputs;
}