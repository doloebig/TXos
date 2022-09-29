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

#ifndef _PortsImpl_h_
#define _PortsImpl_h_

#include "Arduino.h"

class PortsImpl {

    public:
        PortsImpl();

        void portInit( uint8_t p, uint8_t t) const;
        void portSet( uint8_t p, uint8_t s) const;
        uint8_t portGet( uint8_t p) const;
};

#endif
