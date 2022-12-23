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

/*
    The base class for all modules.
 */

#ifndef _Module_h_
#define _Module_h_

#include "TXos.h"
#include "TableEditable.h"
#include "Controls.h"

typedef uint8_t moduleType_t;

typedef uint8_t moduleSize_t;

#define MODULE_INVALID_TYPE             ((moduleType_t)0)

/* List all available modules here */

/* System modules */
#define MODULE_SYSTEM_SETUP_TYPE        ((moduleType_t)1)
#define MODULE_MODEL_SELECT_TYPE        ((moduleType_t)2)
#define MODULE_SERVO_MONITOR_TYPE       ((moduleType_t)3)
#define MODULE_SWITCH_MONITOR_TYPE      ((moduleType_t)4)
#define MODULE_CAL_STICKS_TYPE          ((moduleType_t)5)
#define MODULE_CAL_TRIM_TYPE            ((moduleType_t)6)
#define MODULE_BIND_TYPE                ((moduleType_t)7)
#define MODULE_RANGE_TEST_TYPE          ((moduleType_t)8)
#define MODULE_VCC_MONITOR_TYPE         ((moduleType_t)9)
#define MODULE_STATISTICS_TYPE          ((moduleType_t)10)

/* Model specific modules */
#define MODULE_MODEL_TYPE               ((moduleType_t)50)
#define MODULE_ENGINE_CUT_TYPE          ((moduleType_t)51)
#define MODULE_DUAL_EXPO_TYPE           ((moduleType_t)52)
#define MODULE_TIMER_TYPE               ((moduleType_t)53)
#define MODULE_PHASES_TYPE              ((moduleType_t)54)
#define MODULE_SERVO_REVERSE_TYPE       ((moduleType_t)55)
#define MODULE_SERVO_SUBTRIM_TYPE       ((moduleType_t)56)
#define MODULE_SERVO_LIMIT_TYPE         ((moduleType_t)57)
#define MODULE_PHASES_TRIM_TYPE         ((moduleType_t)58)
#define MODULE_SWITCHED_CHANNELS_TYPE   ((moduleType_t)59)
#define MODULE_ASSIGN_INPUT_TYPE        ((moduleType_t)60)
#define MODULE_SERVO_REMAP_TYPE         ((moduleType_t)61)
#define MODULE_ANALOG_SWITCH_TYPE       ((moduleType_t)62)
#define MODULE_CHANNEL_RANGE_TYPE       ((moduleType_t)63)
#define MODULE_CHANNEL_REVERSE_TYPE     ((moduleType_t)64)
#define MODULE_MIXER_TYPE               ((moduleType_t)65)
#define MODULE_CHANNEL_DELAY_TYPE       ((moduleType_t)66)
#define MODULE_LOGIC_SWITCH_TYPE        ((moduleType_t)67)

class Module : public TableEditable {

    private:
        const char *moduleName;
        moduleType_t moduleType;

        Module *menuNext = nullptr;
        Module *runlistNext = nullptr;

    public:
        Module( moduleType_t type, const char *name);
        friend class ModuleManager;
        friend class Menu;
        
        /* */
        virtual void run( Controls &controls) = 0;

        /* Called after model load */
        virtual void init() { /* noop */ }

        /* Called whenever a flight phase changes. 
         * The overrides are automatically defined in
         * Phases.h NO_CONFIG, PHASED_CONFIG an NON_PHASED_CONFIG macros.
         */
        virtual void switchPhase(phase_t ph) = 0;

        /* Set default values for all configuration data of a module. */
        virtual void setDefaults() = 0;

        /* Get a pointer to the modules configuration data. */
        virtual uint8_t *getConfig() = 0;

        /* Returns the size of the modules configuration data. */
        virtual moduleSize_t getConfigSize() = 0;

        /* Get the modules configuration type identifier.  */
        moduleType_t getConfigType() const;

        /* From Interface TableEditable */
        const char *getName() final;
};

#include "Phases.h"

#endif
