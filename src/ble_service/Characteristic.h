
#include "ble/BLE.h"
#include <cstddef>
/**
     * Read, Write, Notify, Indicate  Characteristic declaration helper.
     *
     * @tparam T type of data held by the characteristic.
     */
    template <typename T, unsigned NUM_ELEMENTS>
    class NotifyCharacteristic : public GattCharacteristic {
    public:
        /**
         * Construct a characteristic that can be read or written and emit
         * notification or indication.
         *
         * @param[in] uuid The UUID of the characteristic.
         * @param[in] initial_value Initial value contained by the characteristic.
         */ 
        NotifyCharacteristic<T, NUM_ELEMENTS>(
        const UUID    &uuid,
        T valuePtr[NUM_ELEMENTS],
        uint8_t additionalProperties = BLE_GATT_CHAR_PROPERTIES_NONE,
        GattAttribute *descriptors[] = nullptr,
        unsigned numDescriptors = 0
           ): GattCharacteristic(
                /* UUID */ uuid,
                /* Initial value */ reinterpret_cast<uint8_t *>(valuePtr),
                /* Value size */ sizeof(T) * NUM_ELEMENTS,
                /* Value capacity */ sizeof(T) * NUM_ELEMENTS,
                /* Properties */ GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY,
                /* Descriptors */ descriptors,
                /* Num descriptors */ numDescriptors
            )
            {
        }

        /**
         * Get the value of this characteristic.
         *
         * @param[in] server GattServer instance that contain the characteristic
         * value.
         * @param[in] dst Variable that will receive the characteristic value.
         *
         * @return BLE_ERROR_NONE in case of success or an appropriate error code.
         */
        ble_error_t get(GattServer &server, T& dst) const
        {
            uint16_t value_length = sizeof(dst);
            return server.read(getValueHandle(), &dst, &value_length);
        }

        /**
         * Assign a new value to this characteristic.
         *
         * @param[in] server GattServer instance that will receive the new value.
         * @param[in] value The new value to set.
         * @param[in] local_only Flag that determine if the change should be kept
         * locally or forwarded to subscribed clients.
         */
        ble_error_t set(
            GattServer &server, const uint8_t value[], bool local_only = false
        ) const {
            //printf("%d%d%d", value[0],value[1],value[2]);
            return server.write(getValueHandle(),  value, sizeof(T) * NUM_ELEMENTS, local_only);
        }

    private:
        uint8_t _value;
    };