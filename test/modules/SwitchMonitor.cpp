
#include "SwitchMonitor.h"

SwitchMonitor::SwitchMonitor( Controls &controls) : Module( MODULE_SWITCH_MONITOR_TYPE, TEXT_MODULE_SWITCHES) , current( controls){

}

void SwitchMonitor::run( Controls &controls) {

    current = controls;
}

void SwitchMonitor::setDefaults() {

    /* no-op */
}

moduleSize_t SwitchMonitor::getConfigSize() {

    /* no-op */
    return 0;
}

uint8_t *SwitchMonitor::getConfig() {

    /* no-op */
    return nullptr;
}

/* From TableEditable */

bool SwitchMonitor::hasChanged() {

    return true;
}

uint8_t SwitchMonitor::getItemCount() {

    return SWITCHES;
}

const char *SwitchMonitor::getItemName( uint8_t row) {

    switch( current.switchConfGet( row)) {
        case SW_CONF_UNUSED:
            return "---";

        case SW_CONF_2STATE:
            return "BI ";

        case SW_CONF_3STATE:
            return "TRI";

        case SW_CONF_CONTROL:
            return "LOG";
    }

    return "---";
}

uint8_t SwitchMonitor::getValueCount() {

    return 1;
}

void SwitchMonitor::getValue( uint8_t row, uint8_t col, Cell *cell) {

    cell->setInt8( current.switchGet( row), 0, 3);
}

void SwitchMonitor::setValue( uint8_t row, uint8_t col, Cell *cell) {

    /* no-op */
}