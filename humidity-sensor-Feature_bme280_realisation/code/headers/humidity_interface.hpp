#pragma once
#include <hwlib.hpp>

namespace r2d2::humidity {
    class humidityInterface {
    public:
        virtual int readHumidity();
    };
}; // namespace r2d2::humidity