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

#include "Phases.h"
#include "ModuleManager.h"

extern ModuleManager moduleManager;

const char *phaseNames[TEXT_PHASES_count] = {
    TEXT_PHASE_NORMAL,
    TEXT_PHASE_THERMAL,
    TEXT_PHASE_SPEED,
    TEXT_PHASE_START,
    TEXT_PHASE_LAND,
    TEXT_PHASE_ACRO,
    TEXT_PHASE_ACRO2
};

Phases::Phases() : Module( MODULE_PHASES_TYPE, TEXT_MODULE_PHASES) {

    setDefaults();
}

void Phases::run( Controls &controls) {

    switchState_t state;

    if( CFG->sw == SWITCH_NONE) {
        return;
    }

    state = controls.switchGet( CFG->sw);

    if( state < PHASES && state != phase) {
        phase = state;
        LOGV("Phases::run: switch to phase %d\n", phase);
        moduleManager.switchPhase( phase);
    }
}

void Phases::setDefaults() {

    INIT_NON_PHASED_CONFIGURATION(

        CFG->sw = SWITCH_NONE;

        phase = 0;

        for( phase_t p = 0; p < PHASES; p++) {
            CFG->phaseName[p] = p;
        }

    )
}

phase_t Phases::getPhase() {

    return phase;
}

const char *Phases::getPhaseName() {

    return phaseNames[CFG->phaseName[phase]];
}

/* From Module */

moduleSize_t Phases::getConfigSize() {

    return (moduleSize_t)sizeof( configuration);
}

uint8_t *Phases::getConfig() {

    return (uint8_t*)&configuration;
}

/* From TableEditable */

uint8_t Phases::getRowCount() {

    /* One for the switch + number of phases */
    return 1 + PHASES;
}

const char *Phases::getRowName( uint8_t row) {

    if( row == 0) {
        return TEXT_SWITCH;
    }

    strcpy( phaseText, "Ph-");
    phaseText[3] = '0'+row-1;
    phaseText[4] = '\0';

    return phaseText;
}

uint8_t Phases::getColCount( uint8_t row) {

    return 1;
}

void Phases::getValue( uint8_t row, uint8_t col, Cell *cell) {

    if( row == 0) {
        cell->setSwitch( 7, CFG->sw);
    } else {
        cell->setList( 6, phaseNames, TEXT_PHASES_count, CFG->phaseName[row-1]);
    }
}

void Phases::setValue( uint8_t row, uint8_t col, Cell *cell) {

    if( row == 0) {
        CFG->sw = cell->getSwitch();
    } else {
        CFG->phaseName[row-1] = cell->getList();
    }
}