#include "bme280.hpp"


r2d2::humidity::bme280::bme280 (r2d2::i2c::i2c_bus_c I2CBus):
I2CBus{I2CBus}
{}

int r2d2::humidity::bme280::readHumidity(){
    I2CBus.write(SensorI2CAdress, SensorRegisterHumidity, 1);
    I2CBus.read(SensorI2CAdress, DataReceiveHumidity, 2);
    uint16_t data = (uint16_t) (DataReceiveHumidity[0] << 8 | DataReceiveHumidity[1]);
return data;
}
