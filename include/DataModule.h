#ifndef DataModule_h
#define DataModule_h

#include "Module.h"

class DataModule : public Module {
    public:
        DataModule();

        void setup() override;
        void update(const unsigned long t) override;

        void setTemp(float temp);
        void setHumidity(float hum);

        float getTemp() const;
        float getHumidity() const;
    
    private:
        float _temp;
        float _humidity;
};

#endif