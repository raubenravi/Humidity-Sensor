#pragma once
#include <humidity_interface.hpp>
#include <hwlib.hpp>

namespace r2d2::humidity {

    // the standard value acording to thedatasheet
    // minim time between read outs
    constexpr uint16_t DTH22MinimumInterval = 2000;
    // stadnart waiting signals
    constexpr uint8_t DTH22MSensorwait1 = 1;
    constexpr uint8_t DTH22MSensorwait2 = 40;
    // the time to decide if a bit is a zero or a one
    constexpr DTH22MBitlength = 60;

    /**
     * @brief enum with states fot the readsensor function
     *
     */
    enum States {
        waitToStart,
        EndOfTransmision,
        Beging,
        waitDataBitToStop,
        waitDataBitToStart
    };

    class DTH22 : public humidityInterface {
    private:
        uint16_t TimepStamMeasurment = 0;
        uint8_t humidity = 0;
        hwlib::pin_in_out &dataPin;
        void readSensor();

    public:
        /**
         *@brief constructor of the humidity class
         *
         * @param datapin this pin wil send and receive data, it should be
         *connected with a 10k resitor to 5v
         *
         */
        DTH22(hwlib::pin_in_out &dataPin);
        /**
         * @brief give the humidity out of the sensor
         *
         * @details
         * reads the humidty from the sensor,
         * if this function is is called within the minimum time interval,
         * it will return the measured humidity.
         *
         * @return The percentage of humidity.
         */

        int readHumidity() override;
    };
} // namespace r2d2::humidity