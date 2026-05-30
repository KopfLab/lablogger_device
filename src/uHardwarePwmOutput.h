#pragma once

#include "uTypedef.h"
#include "enums.h"
#include "Particle.h"

class ThardwarePwmOutput : public TmenuHandle
{

private:
    // variables
    uint8_t Fpin = 0;
    bool Finitialized = false;

    // constants
    const static dtypes::uint8 pwmResolution = 255; // 8-bit

public:
    // sdds vars
    sdds_var(enums::ToffOn, state);
    sdds_var(Tuint8, pwm, pwmResolution);

    // constructor
    ThardwarePwmOutput()
    {
        // events
        on(state)
        {
            analogWrite(Fpin, (state == enums::ToffOn::on) ? pwm.value() : 0);
        };

        on(pwm)
        {
            state.signalEvents();
        };
    }

    // initialize with pin number
    void init(uint8_t _pin)
    {
        Fpin = _pin;
        pinMode(Fpin, OUTPUT);
        analogWrite(Fpin, 0);
    }
};
