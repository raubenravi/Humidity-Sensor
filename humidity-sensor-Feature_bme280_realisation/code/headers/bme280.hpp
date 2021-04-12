#pragma once

#include <humidity_interface.hpp>
#include <hwlib.hpp>
#include <i2c_bus.hpp>

namespace r2d2::humidity {

    constexpr uint8_t SENSOR_I2C_ADRESS_BME280 = 0x76;

    /**
     * @brief all the register of the bme280 that are used
     */
    enum Bme280Registers : uint8_t {
        /**
         * @brief the measurmend registers
         */
        MEAS = 0xF4,
        /**
         * @brief the reset registers
         */
        RESET = 0xE0,
        /**
         * @brief the control humidity registers
         */
        CONTROL_HUMIDITY = 0xF2,
        /**
         * @brief the humidity MSB register that contains the raw measment data
         * @details this register contains the raw data of the humidty
         * measurement. the register next to this one contains the LSb byte of
         * the raw data.
         */
        HUMIDITY = 0xFD,
        /**
         * @brief the tempature MSB register that contains the raw measment data
         * @details this register contains the raw data of the tempature
         * measurement. the register next to this one contains the LSb byte of
         * the raw data.
         */
        TEMPATURE = 0xFA,
        /**
         * @brief the triming parameter wich coontains the data for T1
         */
        TRIM_T1 = 0x88,
        /**
         * @brief the triming parameter wich coontains the data for H1
         */
        TRIM_H1 = 0xA1,
        /**
         * @brief the triming parameter wich coontains the data for H2
         */
        TRIM_H2 = 0xE1
    };
    // this are all the revelant settings for the bme280 esnor
    enum Bme280Settings : uint8_t {
        /**
         * @brief data for configuartion for the humidity measument
         * @details this sets oversampeling for humidity to 16x
         */
        SETTING_HUMIDITY = 5,
        /**
         * @brief data for configuartion for the measument
         * @details this sets oversampeling to 4x for tempature and normal mode
         * for measument
         */
        MEAS_DEFAULT = 131,
        /**
         * @brief reset value for reseting
         */
        RESET_SETTING = 0xB6
    };

    class bme280 : public humidityInterface {
    private:
        r2d2::i2c::i2c_bus_c I2CBus;
        uint8_t DataReceiveHumidity[2];
        // a global according to the datasheet that is needed for calculations
        int32_t t_fine;

        // trimmebles for the sensor
        // each sensor elemnt is is unique and because of that also the
        // trimmebles are different per sensor
        uint16_t dig_T1;
        int16_t dig_T2;
        int16_t dig_T3;
        uint8_t dig_H1;
        int16_t dig_H2;
        uint8_t dig_H3;
        int16_t dig_H4;
        int16_t dig_H5;
        int16_t dig_H6;

        void reset();
        void calibrate();
        uint32_t readTempatureRawData();
        uint32_t readHumidityRawData();

    public:
        /**
         * @brief Creates bme280 i2c sensor class
         * @param I2CBus I2c bus from the r2d21 i2c library and not hwlib
         */
        bme280(r2d2::i2c::i2c_bus_c I2CBus);
        /**
         * @brief Reads humidity out of the sensor
         * @return The percentage of humidity with 3 decimals
         */
        int readHumidity() override;
        /**
         * @brief Reads humidity out of the sensor
         * @return The percentage of humidity with 2 decimals
         */
        int32_t readTempature();
        /**
         * @brief initialise the sensor
         * @details this function will reset the function and set it to normal
         * mode. all the registers regarding tempature and humidity measument
         * will also be set. after it set the sensor in the prefered settings,
         * it will read the trimming data what is required to proces the raw
         * data
         */
        void init();
    };
} // namespace r2d2::humidity