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

#include "Timer.h"
#include "Buzzer.h"

extern Buzzer buzzer;

const buzzerCmd_t SoundMinute[] = {
  BUZZER_PLAY( 2),
  BUZZER_STOP()
};

const buzzerCmd_t SoundSecond[] = {
  BUZZER_PLAY( 1),
  BUZZER_STOP()
};

const buzzerCmd_t SoundZero[] = {
  BUZZER_PLAY( 5),
  BUZZER_STOP()
};

Timer::Timer() : Module( MODULE_TIMER_TYPE, TEXT_MODULE_TIMER) {

    setDefaults();
}

void Timer::reset() {

    ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {
        countdown_sec = CFG->time_sec;
        lastMillis = 0;
    }
    LOGV("Timer::reset: %d\n", countdown_sec);
}

uint16_t Timer::timeSec() {

    uint16_t t;

    ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {
        t = countdown_sec;
    }

    return t;
}

char *Timer::timeStr() {

    uint16_t t;
    uint8_t min;
    uint8_t sec;

    ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {
        t = countdown_sec;
    }

    min = t / 60;
    sec = t % 60;

    if(min < 10) {
        timeAsString[0] = ' ';
    } else {
        timeAsString[0] = '0' + min / 10;
    }

    timeAsString[1] = '0' + min % 10;
    timeAsString[2] = ':';
    timeAsString[3] = '0' + sec / 10;
    timeAsString[4] = '0' + sec % 10;
    timeAsString[5] = '\0';
    
    return timeAsString;
}

/* From Module */

void Timer::run( Controls &controls) {

    long c;
    bool update = false;

    if( controls.evalSwitches( CFG->swState) && countdown_sec > 0) {
        ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {
            c = millis();
            if( c > lastMillis +1500) {
                lastMillis = c;
                countdown_sec--;
                update = true;
            } else if(c > lastMillis +1000) {
                lastMillis += 1000;
                countdown_sec--;
                update = true;
            }
        }

        if( update) {
            if( countdown_sec == 0) {
                LOG("SoundZero\n");
                buzzer.play( SoundZero);
            } else if( countdown_sec <= 10) {
                LOG("SoundSecond\n");
                buzzer.play( SoundSecond);
            } else if( (countdown_sec % 60) == 0) {
                LOG("SoundMinute\n");
                buzzer.play( SoundMinute);
            }
        }
    }
}

void Timer::init() {

    reset();
    LOGV("Timer::init: %d\n", countdown_sec);
}

void Timer::setDefaults() {

    INIT_NON_PHASED_CONFIGURATION(

        INIT_SWITCH( CFG->swState);
        CFG->time_sec = 0;
    )

    reset();
}

/* From TableEditable */

uint8_t Timer::getRowCount() {

    return 2;
}

const char *Timer::getRowName( uint8_t row) {

    if( row == 0) {
        return TEXT_SWITCH;
    }

    return TEXT_TIME;
}

uint8_t Timer::getColCount( uint8_t row) {

    if( row == 0) {
        return 1;
    }

    return 3;
}

void Timer::getValue( uint8_t row, uint8_t col, Cell *cell) {

    uint8_t min;
    uint8_t sec;
    
    if( row == 0) {
        cell->setSwitchState( 7, CFG->swState);
    } else {
        if( col == 0) {
            min = CFG->time_sec / 60;
            cell->setInt8( 7, min, 2, 0, 59);
        } else if( col == 1) {
            cell->setLabel( 9, ":", 1);
        } else if( col == 2) {
            sec = CFG->time_sec % 60;
            cell->setInt8( 10, sec, 2, 0, 59);
        }
    }
}

void Timer::setValue( uint8_t row, uint8_t col, Cell *cell) {

    uint8_t min;
    uint8_t sec;

    if( row == 0) {
        CFG->swState = cell->getSwitchState();
    } else {
        
        min = CFG->time_sec / 60;
        sec = CFG->time_sec % 60;

        if( col == 0) {
            min = cell->getInt8();
        } else if ( col == 2) {
            sec = cell->getInt8();
        }

        CFG->time_sec = (uint16_t)60 * min + sec;
    }
}