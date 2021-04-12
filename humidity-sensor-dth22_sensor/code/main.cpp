#include "hwlib.hpp"
#include "DTH22.hpp"

int main(void) {
  // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;
    hwlib::wait_ms(1000);
    for (;;){
        auto dataPin =  hwlib::target::pin_in_out( hwlib::target::pins::d12 );
        auto dth22 = r2d2::humidity::DTH22(dataPin) ;
        hwlib::cout << dth22.readHumidity() << '\n';
        hwlib::wait_ms(1000);
    }
}