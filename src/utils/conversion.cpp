#include "conversion.h" // Include the header for the function declaration

// Function definition
float get_analog_value(int measurement, float databits, float vref, float gain) {
    /* calculate the analog value from the measurement */
    float voltage = (float)measurement / databits - 1;
    voltage = voltage * vref / gain;
    voltage *= 1000;

    // printf("analog %.3f\n", voltage); // Note: %.3f for float precision
    return voltage;
}