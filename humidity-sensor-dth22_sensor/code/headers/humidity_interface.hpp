#pragma once

namespace r2d2::humidity {
    class humidityInterface {
    public:
        virtual int readHumidity() = 0;
    };
};