#ifndef CONVERSION_H
#define CONVERSION_H

// Function declaration (prototype)
std::vector<float> get_analog_inputs(std::vector<std::array<uint8_t,3>> byte_inputs, float databits, float vref, float gain);

#endif // CONVERSION_H
