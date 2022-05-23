
#ifndef _ServoReverse_h_
#define _ServoReverse_h_

#include "TXos.h"
#include "Module.h"

typedef struct servoReverse_t {

        channelBits_t revBits;

} servoReverse_t;

class ServoReverse : public Module {

    private:
        servoReverse_t cfg;

    public:
        ServoReverse();
        void run( channelSet_t &channels);
        void setDefaults();

        moduleSize_t getConfigSize();
        uint8_t *getConfig();

        /* From TableEditable */
        uint8_t getItemCount();
        const char *getItemName( uint8_t row);
        uint8_t getValueCount();
        void getValue( uint8_t row, uint8_t col, Cell *cell);
        void setValue( uint8_t row, uint8_t col, Cell *cell);
};

#endif
