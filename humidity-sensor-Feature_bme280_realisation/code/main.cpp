#include "bme280.hpp"
#include "hwlib.hpp"
#include <humidity_interface.hpp>
#include <i2c_bus.hpp>

int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;
    hwlib::wait_ms(1000);
    auto bus = r2d2::i2c::i2c_bus_c(
        r2d2::i2c::i2c_bus_c::interface::interface_1, 100'000);
    auto humidity = r2d2::humidity::bme280(bus);
    humidity.init();
    for (;;) {
        hwlib::cout << humidity.readHumidity() << "   "
                    << humidity.readTempature() << '\n';

        hwlib::wait_ms(1500);
    }
}