/*
  TXos. A remote control transmitter OS.

  MIT License

  Copyright (c) 2023 wlowi

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include "ChannelDelay.h"

extern const char* const LogicalChannelNames[LOGICAL_CHANNELS];

ChannelDelay::ChannelDelay() : Module( MODULE_CHANNEL_DELAY_TYPE, TEXT_MODULE_CHANNEL_DELAY) {

    setDefaults();
}

/* From Module */

void ChannelDelay::exportConfig( Comm *exporter, uint8_t *config, moduleSize_t configSz) const {

    const channelDelay_t *cfg = (channelDelay_t*)config;

    exporter->openSub( COMM_SUBPACKET_CHANNEL_DELAY );
    exporter->addIntArr( COMM_FIELD_DELAY_ARRAY, (const byte*)cfg->posDelay_sec, sizeof(cfg->posDelay_sec), MIX_CHANNELS);
    exporter->addIntArr( COMM_FIELD_NEG_DELAY_ARRAY, (const byte*)cfg->negDelay_sec, sizeof(cfg->negDelay_sec), MIX_CHANNELS);
    exporter->close();
}

void ChannelDelay::run( Controls &controls) {

#define SCALING_F (10)

    int16_t targetPosition10;  // scaled by 10 for increased precision
    int16_t stepPosition10;    // same here
    int16_t delay_msec;

    for( uint8_t mix = 0; mix < MIX_CHANNELS; mix++) {

        /* The target position in 1/10 % scaled to 1/100 % */
        targetPosition10 = controls.logicalGet( mix) * SCALING_F;

        if( (CFG->posDelay_sec[mix] > 0) && (targetPosition10 > lastChannelValue10[mix]) ) {
            // posDelay_sec is a scaled float in 1/10 sec resolution. Convert to msec.
            delay_msec = CFG->posDelay_sec[mix] * 100;
            stepPosition10 = (int16_t)((int32_t)((CHANNELVALUE_MAX - CHANNELVALUE_MIN) * SCALING_F) * PPM_FRAME_TIME_usec / 1000 / delay_msec);
            lastChannelValue10[mix] += stepPosition10;
            if( lastChannelValue10[mix] > targetPosition10) { // Do not exceed targeted value.
                lastChannelValue10[mix] = targetPosition10;
            }

        } else if( (CFG->negDelay_sec[mix] > 0) && (targetPosition10 < lastChannelValue10[mix]) ) {
            delay_msec = CFG->negDelay_sec[mix] * 100;
            stepPosition10 = (int16_t)((int32_t)((CHANNELVALUE_MAX - CHANNELVALUE_MIN) * SCALING_F) * PPM_FRAME_TIME_usec / 1000 / delay_msec);
            lastChannelValue10[mix] -= stepPosition10;
            if( lastChannelValue10[mix] < targetPosition10) {
                lastChannelValue10[mix] = targetPosition10;
            }

        } else {
            lastChannelValue10[mix] = targetPosition10;
            continue;
        }

        controls.logicalSet( mix, (channelValue_t)(lastChannelValue10[mix] / SCALING_F));
    }
}

void ChannelDelay::setDefaults() {

    INIT_NON_PHASED_CONFIGURATION(

        for( channel_t ch = 0; ch < MIX_CHANNELS; ch++) {
            CFG->posDelay_sec[ch] = 0;
            CFG->negDelay_sec[ch] = 0;
        }
    )

    for( channel_t ch = 0; ch < MIX_CHANNELS; ch++) {
        lastChannelValue10[ch] = 0;
    }
}

/* From TableEditable */

uint8_t ChannelDelay::getRowCount() {

    return MIX_CHANNELS;
}

const char *ChannelDelay::getRowName( uint8_t row) {

    return LogicalChannelNames[row];
}

uint8_t ChannelDelay::getColCount( uint8_t row) {

    return 2;
}

void ChannelDelay::getValue( uint8_t row, uint8_t col, Cell *cell) {

    if( col == 0) {
        cell->setFloat1( 4, CFG->posDelay_sec[row], 4, 0, CHANNELDELAY_MAX_SEC * 10 /* 10.0 sec */);
    } else if( col == 1) {
        cell->setFloat1( 9, CFG->negDelay_sec[row], 4, 0, CHANNELDELAY_MAX_SEC * 10 /* 10.0 sec */);
    }
}

void ChannelDelay::setValue( uint8_t row, uint8_t col, Cell *cell) {

    if( col == 0) {
        CFG->posDelay_sec[row] = cell->getFloat1();
    } else if( col == 1) {
        CFG->negDelay_sec[row] = cell->getFloat1();
    }
}
