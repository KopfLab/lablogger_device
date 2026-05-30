#pragma once

#include "uTypedef.h"
#include "enums.h"
#include "uMultask.h"
#include "uParticleSystem.h"

// hardware parts
#include "uHardwareSensorVoltage.h"
#include "uHardwareDisplay.h"

// hardware constants
#define LABLOGGER_VOLTAGE_PIN A0
#define LABLOGGER_VOLTAGE_DIVIDER_REF 3.3
#define LABLOGGER_VOLTAGE_DIVIDER_R1 150.0
#define LABLOGGER_VOLTAGE_DIVIDER_R2 10.0
#define LABLOGGER_VOLTAGE_SCHOTTKY_DROP 0.0 // does not have a diode
#define LABLOGGER_CONTROLLER_VERSION_PIN1 D2
#define LABLOGGER_CONTROLLER_VERSION_PIN2 D3

/**
 * @brief hardware handler
 * uses a TmenuHandle for the convenience of sdds_vars but is not usually
 * included in the tree except for stand-alone testing
 */
class ThardwareLablogger : public TmenuHandle
{

private:
    // in case it is used in a tree
    Tmeta meta() override { return Tmeta{TYPE_ID, 0, "HARDWARE"}; }

public:
    // pcb versions
    class Tpcbs : public TmenuHandle
    {
    public:
        sdds_var(Tuint8, controller, sdds::opt::readonly);
    };
    sdds_var(Tpcbs, pcbVersions);
    // display
    sdds_var(ThardwareDisplay, display);
    // signals
    sdds_var(ThardwareSensorVoltage, voltage);

    // constructor
    ThardwareLablogger()
    {

        // run during setp
        on(sdds::setup())
        {

            // define pins
            pinMode(LABLOGGER_CONTROLLER_VERSION_PIN1, INPUT);
            pinMode(LABLOGGER_CONTROLLER_VERSION_PIN2, INPUT);

            // initialize hardware components (defines other pins)
            display.init(particleSystem().version.value());
            voltage.init(LABLOGGER_VOLTAGE_PIN, LABLOGGER_VOLTAGE_DIVIDER_REF, LABLOGGER_VOLTAGE_DIVIDER_R1, LABLOGGER_VOLTAGE_DIVIDER_R2, LABLOGGER_VOLTAGE_SCHOTTKY_DROP);

            // controller board version
            uint8_t b0 = digitalRead(LABLOGGER_CONTROLLER_VERSION_PIN1) ? 1 : 0;
            uint8_t b1 = digitalRead(LABLOGGER_CONTROLLER_VERSION_PIN2) ? 1 : 0;
            uint8_t version = (b1 << 1) | b0;
            pcbVersions.controller = version + 1;
        };

        // turn the voltage reader on
        voltage.state = enums::ToffOn::on;
    }
};
