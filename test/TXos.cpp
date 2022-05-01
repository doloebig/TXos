/*
 * TXos - RC Transmitter OS
 *
 * D0   RX            - USB
 * D1   TX            - USB
 * D2   Clk           - Rotary Encoder
 * D3   Dir           - Rotary Encoder
 * D4   Switch        - Rotary Encoder
 * D5   PPM Out
 * D6   Digital 1
 * D7   Digital 2
 * D8   Digital 3
 * D9   Digital 4
 * D10  Digital 5
 * D11  MOSI          - ICSP
 * D12  MISO          - ICSP
 * D13  SCK           - ICSP
 * 
 * A0   Analog 1
 * A1   Analog 2
 * A2   Analog 3
 * A3   Analog 4
 * A4   SDA           - TWI (LCD)
 * A5   SCL           - TWI (LCD)
 * A6   Analog 5
 * A7   Vcc Monitor
 * 
 */

#include "TXos.h"

#include "EEPROM.h"

#include "Controls.h"
#include "Output.h"
#include "UserInterface.h"

#include "Module.h"
#include "ModuleManager.h"

#include "ServoReverse.h"

channelSet_t channels;
Controls controls;
Output output;
UserInterface userInterface;
ModuleManager moduleManager;

void setup( void) {

    moduleManager.Add( new ServoReverse());

    controls.init();
    output.init();
    userInterface.init();
}

void loop( void) {

    controls.GetControlValues( channels);
    moduleManager.RunModules( channels);
    output.setChannels( channels);

    userInterface.handle();
}