#include "DTH22.hpp"
#include "hwlib.hpp"

namespace r2d2::humidity {

    r2d2::humidity::DTH22::DTH22(hwlib::pin_in_out &dataPin)
        : dataPin{dataPin} {
    }

    int r2d2::humidity::DTH22::readHumidity() {
        // decide if there is new measument within the minimum time interval
        // tminimum time interval according to the datasheet is 2s
        if (r2d2::humidity::DTH22::TimepStamMeasurment - hwlib::now_us() >=
            DTH22MinimumInterval) {
            r2d2::humidity::DTH22::readSensor();
            return humidity;
        } else {
            return humidity;
        }
    }

    void r2d2::humidity::DTH22::readSensor() {
        WDT->WDT_MR = WDT_MR_WDDIS;
        hwlib::wait_ms(1000);
        uint_fast8_t i = 0;
        uint_fast16_t dataByte = 0;
        uint_fast8_t bit = 0;
        uint_fast64_t timedata;
        uint_fast64_t timeBegin;
        States state = Beging;

        while (true) {
            switch (state) {

            case Beging:
                // writing the start signal to the sensor
                dataPin.direction_set_output();
                dataPin.direction_flush();
                dataPin.write(0);
                dataPin.flush();
                hwlib::wait_ms(1);
                dataPin.direction_set_output();
                dataPin.direction_flush();
                dataPin.write(1);
                dataPin.flush();
                hwlib::wait_us(40);
                dataPin.direction_set_input();
                dataPin.direction_flush();
                hwlib::wait_us(160);
                state = waitToStart;
                break;

            case waitToStart:
                // wait for the start signal
                if (dataPin.read() == 0) {
                    state = waitDataBitToStart;
                }
                break;
            case waitDataBitToStart:
                // the first 2 bytes are humidity data
                // that`s way it will stop at 16 bits
                if (i == 16) {
                    humidity = dataByte / 10; // value from datasheet
                    TimepStamMeasurment = hwlib::now_us();
                    dataPin.direction_set_output();
                    dataPin.write(1);
                    dataPin.flush();
                    return;
                }
                // reads start signal of single bit
                if (dataPin.read() == 1) {
                    timeBegin = hwlib::now_us();
                    state = waitDataBitToStop;
                }
                break;

            case waitDataBitToStop:
                // reads the faling edge of a bit/
                if (dataPin.read() == 0) {
                    //
                    timedata = hwlib::now_us() - timeBegin;
                    // a bit is a one or a zero according to the time how long a
                    // signal is high
                    (timedata < DTH22MBitlength) ? (bit = 0) : (bit = 1);
                    dataByte = dataByte << 1;
                    dataByte = dataByte | bit;
                    i++;
                    state = waitDataBitToStart;
                }
                break;

            default:
                break;
            }
        }
    }
} // namespace r2d2::humidity