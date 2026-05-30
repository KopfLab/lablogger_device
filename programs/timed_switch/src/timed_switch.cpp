#include "Particle.h"
#include "uTypedef.h"
#include "uCoreEnums.h"
#include "uMultask.h"
#include "uHardwareLablogger.h"
#include "uHardwarePwmOutput.h"
#include "uComponentTimedSwitch.h"
#include "symbols.h"

// hardware
class ThardwareTimedDoubleSwitch : public ThardwareLablogger
{
public:
    // sdds vars
    sdds_var(ThardwarePwmOutput, switch1);
    sdds_var(ThardwarePwmOutput, switch2);

    // constructor
    ThardwareTimedDoubleSwitch()
    {
        // run during setup
        on(sdds::setup())
        {
            // define pins for switches
            switch1.init(A2);
            switch2.init(A5);
        };
    }
};

// FIXME: continue here

/**
 * @brief get the static hadware instance
 */
ThardwareTimedDoubleSwitch &hardware()
{
    static ThardwareTimedDoubleSwitch hardware;
    return hardware;
}

// self-describing data structure (SDDS) tree
class TsddsTree : public TmenuHandle
{
private:
    // timers
    Ttimer FdisplayTimer;

    // display update function
    void refreshDisplay()
    {
        hardware().display.clearDisplay();
        hardware().display.setCursor(0, 0);
        char buf[16]; // snprintf buffer

        // wifi?
        if (particleSystem().internet == TparticleSystem::TinternetStatus::connected)
            hardware().display.drawIcon(wifi_icon, wifi_icon_width);
        else
            hardware().display.drawIcon(no_wifi_icon, no_wifi_icon_width);

        // publishing? (don't show the "not publishing", it's not as informative)
        if (particleSystem().publishing.record == sdds::enums::OnOff::ON)
            hardware().display.drawIcon(publishing_icon, publishing_icon_width);

        // device name
        if (particleSystem().name == "")
            hardware().display.printLine(ThardwareDisplay::headerY, "connecting...");
        else
            hardware().display.printLine(ThardwareDisplay::headerY, particleSystem().name);

        // power
        snprintf(buf, sizeof(buf), "%.1fV", hardware().voltage.value.value());
        hardware().display.printLine(ThardwareDisplay::line1Y, "Power:");
        hardware().display.printLine(ThardwareDisplay::line1Y, buf, ThardwareDisplay::offsetX);

        // switch1 status
        hardware().display.printLine(ThardwareDisplay::line2Y, "Switch #1:");
        if (timedSwitch1.status == TcomponentTimedSwitch::Tstatus::on)
            hardware().display.printLine(ThardwareDisplay::line2Y, timedSwitch1.intensity_percent.to_string() + "%", ThardwareDisplay::offsetX);
        else if (timedSwitch1.status == TcomponentTimedSwitch::Tstatus::off)
            hardware().display.printLine(ThardwareDisplay::line2Y, "off", ThardwareDisplay::offsetX);
        else
            hardware().display.printLine(ThardwareDisplay::line2Y, timedSwitch1.status.to_string(), ThardwareDisplay::offsetX);

        // switch1 state
        if (timedSwitch1.state == TcomponentTimedSwitch::Tstate::on)
            hardware().display.printLine(ThardwareDisplay::line3Y, "always on", ThardwareDisplay::offsetX);
        else if (timedSwitch1.state == TcomponentTimedSwitch::Tstate::off)
            hardware().display.printLine(ThardwareDisplay::line3Y, "always off", ThardwareDisplay::offsetX);
        else if (timedSwitch1.state == TcomponentTimedSwitch::Tstate::schedule)
            hardware().display.printLine(ThardwareDisplay::line3Y, timedSwitch1.scheduleInfo.c_str(), ThardwareDisplay::offsetX);

        // switch2 status
        hardware().display.printLine(ThardwareDisplay::line2Y, "Switch #2:");
        if (timedSwitch2.status == TcomponentTimedSwitch::Tstatus::on)
            hardware().display.printLine(ThardwareDisplay::line2Y, timedSwitch2.intensity_percent.to_string() + "%", ThardwareDisplay::offsetX);
        else if (timedSwitch2.status == TcomponentTimedSwitch::Tstatus::off)
            hardware().display.printLine(ThardwareDisplay::line2Y, "off", ThardwareDisplay::offsetX);
        else
            hardware().display.printLine(ThardwareDisplay::line2Y, timedSwitch2.status.to_string(), ThardwareDisplay::offsetX);

        // switch2 state
        if (timedSwitch2.state == TcomponentTimedSwitch::Tstate::on)
            hardware().display.printLine(ThardwareDisplay::line3Y, "always on", ThardwareDisplay::offsetX);
        else if (timedSwitch2.state == TcomponentTimedSwitch::Tstate::off)
            hardware().display.printLine(ThardwareDisplay::line3Y, "always off", ThardwareDisplay::offsetX);
        else if (timedSwitch2.state == TcomponentTimedSwitch::Tstate::schedule)
            hardware().display.printLine(ThardwareDisplay::line3Y, timedSwitch2.scheduleInfo.c_str(), ThardwareDisplay::offsetX);

        // update dispaly
        hardware().display.drawLayoutLines();
    }

public:
    // sdds vars
    sdds_var(TcomponentTimedSwitch, timedSwitch1);
    sdds_var(TcomponentTimedSwitch, timedSwitch2);

    // constructor
    TsddsTree()
    {
        // make sure hardware is initalized
        hardware();

        // run during setup
        on(sdds::setup())
        {
            // set the hardware for the timed switch
            timedSwitch1.setPwmOutput(&hardware().switch1);
            timedSwitch2.setPwmOutput(&hardware().switch2);
        };

        // display startup complete
        on(hardware().display.startup)
        {
            if (hardware().display.startup == ThardwareDisplay::Tstartup::complete)
            {
                FdisplayTimer.start(0);
            }
        };

        // regular display refresh
        on(FdisplayTimer)
        {
            refreshDisplay();
            hardware().display.action = ThardwareDisplay::Taction::write;
            FdisplayTimer.start(hardware().display.refresh_ms);
        };
    }
} sddsTree;

// serial spike for communication via serial (with baud rate)
#include "uSerialSpike.h"
TserialSpike serialSpike(sddsTree, 115200);

// particle spike for particle communication (with sdds name and version)
#include "uParticleSpike.h"
static TparticleSpike particleSpike(sddsTree, "timed switch (x2)", 103);

// setup
void setup()
{
    // setup particle spike
    particleSpike.setup();

    // add hardware menu after the particle spike so it does not have publish options
    sddsTree.addDescr(hardware());
}

// loop
void loop()
{
    // handle all events
    TtaskHandler::handleEvents();
}
