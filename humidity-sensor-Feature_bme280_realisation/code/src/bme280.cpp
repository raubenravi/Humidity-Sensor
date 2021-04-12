#include "bme280.hpp"

using namespace r2d2::humidity;

r2d2::humidity::bme280::bme280(r2d2::i2c::i2c_bus_c I2CBus) : I2CBus{I2CBus} {
}

void r2d2::humidity::bme280::init() {
    reset();
    calibrate();
}

void r2d2::humidity::bme280::calibrate() {
    // byte to send to sensor
    uint8_t ByteI2cToSensor[1] = {Bme280Registers::TRIM_T1};
    // bit to store the data from the register
    uint8_t DataReceiveTrimming[7];

    // getting the data from the sensor and set the parameter T1 to T3
    // more info at paragraph 4.2.2 from the datasheet
    I2CBus.write(SENSOR_I2C_ADRESS_BME280, ByteI2cToSensor, 1);
    I2CBus.read(SENSOR_I2C_ADRESS_BME280, DataReceiveTrimming, 6);
    dig_T1 = ((uint16_t)DataReceiveTrimming[1] << 8) |
             ((uint16_t)DataReceiveTrimming[0]);
    dig_T2 = (int16_t)(DataReceiveTrimming[3] << 8) |
             ((int16_t)DataReceiveTrimming[2]);
    dig_T3 =
        (int16_t)DataReceiveTrimming[5] << 8 | (int16_t)DataReceiveTrimming[4];

    // getting the data from the sensor and set the parameter H1
    // more info at paragraph 4.2.2 from the datasheet
    ByteI2cToSensor[0] = r2d2::humidity::Bme280Registers::TRIM_H1;
    I2CBus.write(SENSOR_I2C_ADRESS_BME280, ByteI2cToSensor, 1);
    I2CBus.read(SENSOR_I2C_ADRESS_BME280, DataReceiveTrimming, 1);
    dig_H1 = DataReceiveTrimming[0];

    // getting the data from the sensor and set the parameter H2 to H3
    // more info at paragraph 4.2.2 from the datasheet
    ByteI2cToSensor[0] = r2d2::humidity::Bme280Registers::TRIM_H2;
    I2CBus.write(SENSOR_I2C_ADRESS_BME280, ByteI2cToSensor, 1);
    I2CBus.read(SENSOR_I2C_ADRESS_BME280, DataReceiveTrimming, 7);
    dig_H2 = (int16_t)DataReceiveTrimming[1] << 8 | DataReceiveTrimming[0];
    dig_H3 = DataReceiveTrimming[2];
    dig_H4 = (DataReceiveTrimming[3] << 4) | (DataReceiveTrimming[4]);
    dig_H5 = (int16_t)DataReceiveTrimming[5] << 8 | DataReceiveTrimming[4];
    dig_H5 >>= 4;
    dig_H6 = DataReceiveTrimming[6];
}

void r2d2::humidity::bme280::reset() {
    // reset the sensor
    uint8_t BytesI2cToSensor[2] = {Bme280Registers::RESET,
                                   Bme280Settings::RESET_SETTING};
    I2CBus.write(SENSOR_I2C_ADRESS_BME280, BytesI2cToSensor, 2);
    // sensor has to sleep to make changes effective
    BytesI2cToSensor[0] = Bme280Registers::MEAS;
    BytesI2cToSensor[1] = 0;
    I2CBus.write(SENSOR_I2C_ADRESS_BME280, BytesI2cToSensor, 2);
    // set controles to measuments data
    BytesI2cToSensor[0] = Bme280Registers::CONTROL_HUMIDITY;
    BytesI2cToSensor[1] = Bme280Settings::SETTING_HUMIDITY;
    I2CBus.write(SENSOR_I2C_ADRESS_BME280, BytesI2cToSensor, 2);
    // Set`s the sensor to mode 3 so it will periodicly make a measument
    BytesI2cToSensor[0] = Bme280Registers::MEAS;
    BytesI2cToSensor[1] = Bme280Settings::MEAS_DEFAULT;
    I2CBus.write(SENSOR_I2C_ADRESS_BME280, BytesI2cToSensor, 2);
}

uint32_t r2d2::humidity::bme280::readHumidityRawData() {
    // array with byte info wich will be send to the reggister and will be
    // overwritten when it receive the data from the sensor
    uint8_t DataArayi2cBytes[2] = {Bme280Registers::HUMIDITY, 0};
    I2CBus.write(SENSOR_I2C_ADRESS_BME280, DataArayi2cBytes, 1);
    I2CBus.read(SENSOR_I2C_ADRESS_BME280, DataArayi2cBytes, 2);
    uint16_t data = (uint16_t)(DataArayi2cBytes[0] << 8 | DataArayi2cBytes[1]);
    return data;
}

uint32_t r2d2::humidity::bme280::readTempatureRawData() {
    // array with byte info wich will be send to the reggister and will be
    // overwritten when it receive the data from the sensor
    uint8_t DataArayi2cBytes[3] = {Bme280Registers::TEMPATURE, 0, 0};
    I2CBus.write(SENSOR_I2C_ADRESS_BME280, DataArayi2cBytes, 1);
    I2CBus.read(SENSOR_I2C_ADRESS_BME280, DataArayi2cBytes, 3);
    uint32_t data = ((uint32_t)DataArayi2cBytes[0] << 16) |
                    ((uint32_t)DataArayi2cBytes[1] << 8) |
                    ((uint32_t)DataArayi2cBytes[2]);
    data >>= 4;

    return data;
}

int32_t r2d2::humidity::bme280::readTempature() {

    // this code below is copied from the datasheet
    // only the syntax of the datatype is changed
    int32_t var1, var2, T;
    int32_t adc_T = readTempatureRawData();
    uint16_t dig_T1_temp = dig_T1;
    int16_t dig_T2_temp = dig_T2;
    int16_t dig_T3_temp = dig_T3;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1_temp << 1))) *
            ((int32_t)dig_T2_temp)) >>
           11;

    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1_temp)) *
              ((adc_T >> 4) - ((int32_t)dig_T1_temp))) >>
             12) *
            ((int32_t)dig_T3_temp)) >>
           14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T;
}

int r2d2::humidity::bme280::readHumidity() {
    // must be done to calcuate t_fine
    readTempature();
    int32_t adc_H = readHumidityRawData();

    // this code below is copied from the datasheet
    // only the syntax of the datatype is changed
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t)76800));

    v_x1_u32r =
        (((((adc_H << 14) - (((int32_t)dig_H4) << 20) -
            (((int32_t)dig_H5) * v_x1_u32r)) +
           ((int32_t)16384)) >>
          15) *
         (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) *
              (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >>
             10) +
            ((int32_t)2097152)) *
               ((int32_t)dig_H2) +
           8192) >>
          14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                               ((int32_t)dig_H1)) >>
                              4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (uint32_t)(v_x1_u32r >> 12);
}
