
#ifndef _ModelSelect_h_
#define _ModelSelect_h_

#include "TXos.h"
#include "Module.h"
#include "ConfigBlock.h"

class ModelSelect : public Module {

    private:
        char modelNo[3];
        
    public:
        ModelSelect();

        /* From Module */
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
