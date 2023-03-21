#pragma once

class OTA_CONNECTION {
    public:
        void begin();
        void end();
        void loop();
        bool getIsActive();

    private:
        bool isActive = false;
};
