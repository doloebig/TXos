
#ifndef _ServoMonitor_h_
#define _ServoMonitor_h_

#include "Module.h"

class ServoMonitor : public Module {

    NO_CONFIG()

    private:
        Controls &current;

    public:
        explicit ServoMonitor( Controls &controls);
        
        void run( Controls &controls) final;
        void setDefaults() final;

        /* From TableEditable */
        bool isRowEditable( uint8_t row) final { return false; }

        uint8_t getRowCount() final;
        const char *getRowName( uint8_t row) final;
        uint8_t getColCount( uint8_t row) final;
        
        void getValue( uint8_t row, uint8_t col, Cell *cell) final;
        void setValue( uint8_t row, uint8_t col, Cell *cell) final;
        bool hasChanged( uint8_t row, uint8_t col) final;
};

#endif