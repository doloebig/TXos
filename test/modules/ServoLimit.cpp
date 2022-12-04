/*
    TXos. A remote control transmitter OS.

    Copyright (C) 2022 Wolfgang Lohwasser

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include "ServoLimit.h"

ServoLimit::ServoLimit() : Module( MODULE_SERVO_LIMIT_TYPE, TEXT_MODULE_SERVO_LIMIT) {

    setDefaults();
}

/* From Module */

void ServoLimit::run( Controls &controls) {

    channelValue_t v;

    for( channel_t ch = 0; ch < CHANNELS; ch++) {

        v = PCT_TO_CHANNEL( CFG->posLimit_pct[ch]);
        if( controls.analogGet( ch) > v) {
            controls.analogSet( ch, v);
        }

        v = PCT_TO_CHANNEL( CFG->negLimit_pct[ch]);
        if( controls.analogGet( ch) < v) {
            controls.analogSet( ch, v);
        }
    }
}

void ServoLimit::setDefaults() {

    INIT_NON_PHASED_CONFIGURATION(

        for( channel_t ch = 0; ch < CHANNELS; ch++) {
            CFG->posLimit_pct[ch] = PERCENT_MAX_LIMIT;
            CFG->negLimit_pct[ch] = PERCENT_MIN_LIMIT;
        }

    )
}

/* From TableEditable */

uint8_t ServoLimit::getRowCount() {

    return CHANNELS;
}

const char *ServoLimit::getRowName( uint8_t row) {

    return ChannelNames[row];
}

uint8_t ServoLimit::getColCount( uint8_t row) {

    return 2;
}

void ServoLimit::getValue( uint8_t row, uint8_t col, Cell *cell) {

    if( col == 0) {
        cell->setInt8( 4, CFG->negLimit_pct[row], PERCENT_MIN_LIMIT, PERCENT_MAX_LIMIT);
    } else {
        cell->setInt8( 9, CFG->posLimit_pct[row], PERCENT_MIN_LIMIT, PERCENT_MAX_LIMIT);
    }
}

void ServoLimit::setValue( uint8_t row, uint8_t col, Cell *cell) {

    if( col == 0) {
        CFG->negLimit_pct[row] = cell->getInt8();
    } else {
        CFG->posLimit_pct[row] = cell->getInt8();
    }
}
