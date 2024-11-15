# Coding Convention
## 1. Naming Convention
### 1.1 Variables
- Local Variables and Parameters: Use snake_case.
    - Example: sensor_reading
### 1.2 File names
-  If the .cpp file defines a class, it’s common to name the file exactly as the class, especially for classes using CamelCase
## Datatypes
## int over long
- On Cortex-M4, int and long are both 32-bit types with identical performance characteristics, so you can use either based on your style preferences or code readability.
- If you need larger numbers than a 32-bit range allows, use long long but be aware of the performance impact due to software emulation.

## float over double
- Use float for better performance on Cortex-M4 since the FPU is available.
- Use double only if the application requires 64-bit precision and the slower performance is acceptable.

## Assignments explicit instead of implicit
- Example: uint8_t data[4] = {0, 0, 0, 0}; over uint8_t data[4] = {0};
