#ifndef CONVERSION_H
#define CONVERSION_H

#include <cstdio>    // Include this header for printf
#include <vector>    // Include this header for std::vector
#include <array>     // Include this header for std::array
#include <cstdint>   // Include this header for uint8_t (optional, but useful)

// Function declaration (prototype)
std::vector<float> get_analog_inputs(std::vector<std::array<uint8_t,3>> byte_inputs, int databits, float vref, float gain);

#endif // CONVERSION_H
