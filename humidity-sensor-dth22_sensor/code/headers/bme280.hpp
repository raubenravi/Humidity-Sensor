#pragma once

#include <humidity_interface.hpp>
#include <hwlib.hpp>
#include <i2c_bus.hpp>

namespace r2d2::humidity {
    class bme280 : public humidityInterface {
    private:
        r2d2::i2c::i2c_bus_c I2CBus;
        uint8_t SensorI2CAdress = 0xFD;
        uint8_t SensorRegisterHumidity[1] = {0x76};
        uint8_t DataReceiveHumidity[2];

    public:
        /**
         * @brief Creates bme280 i2c sensor class
         * @param I2CBus I2c bus from the r2d21 i2c library and not hwlib
         */
        bme280(r2d2::i2c::i2c_bus_c I2CBus);
        /**
         * @brief Reads humidity out of the sensor
         * @return The percentage of humidity.
         */
        int readHumidity() override;
    };
} // namespace r2d2::humidity