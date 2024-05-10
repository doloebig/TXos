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

#include "InputImpl.h"
#include "PortsImpl.h"

extern InputImpl *inputImpl;
extern PortsImpl *portsImpl;

/* 
 * ADC conversion complete interrupt. 
 */
#if defined(ARDUINO_ARCH_AVR)
ISR(ADC_vect) {

    /* MUST read ADCL first */
    channelValue_t v = ADCL;
    v |= (ADCH << 8);

    /* Disable ADC */
    ADCSRA &= ~_BV(ADEN);
    
    if( inputImpl->mux < inputImpl->adcInputs) {

#ifdef INVERT_CH1
      if( inputImpl->mux == 0) v = ADC_RESOLUTION -v;
#endif
#ifdef INVERT_CH2
      if( inputImpl->mux == 1) v = ADC_RESOLUTION -v;
#endif
#ifdef INVERT_CH3
      if( inputImpl->mux == 2) v = ADC_RESOLUTION -v;
#endif
#ifdef INVERT_CH4
      if( inputImpl->mux == 3) v = ADC_RESOLUTION -v;
#endif

      inputImpl->adcValues[inputImpl->mux] = v;
      
      inputImpl->mux++;
      inputImpl->setMux();      
    } else {
      /* done */
    }
}
#endif

InputImpl::InputImpl( channel_t stickCnt, channel_t trimCnt, channel_t auxCnt,
                      const uint8_t analogPins[],
                      switch_t switches, const switchConf_t *conf,
                      const uint8_t switchPins[])
{
    this->stickCount = stickCnt;
    this->trimCount = trimCnt;
    this->auxCount = auxCnt;
    this->adcInputs = stickCnt + trimCnt + auxCnt;
    this->switches = switches;
    this->switchConf = conf;
    this->analogPins = analogPins;
    this->switchPins = switchPins;

    adcValues = new channelValue_t[adcInputs];

    for( int i=0; i<adcInputs; i++) {
        adcValues[i] = 0;
    }

    init();
}

void InputImpl::init() {

#if defined(ARDUINO_ARCH_AVR)
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {
    
        /* Disable power reduction for ADC */
        PRR0 &= ~_BV(PRADC);
         
        /* REFS1 = 0, REFS0 = 1   ==>   VCC with ext. cap. on AREF */
        ADMUX = _BV(REFS0);
        
        /* Prescaler /128   ==>   16MHz / 128 = 125KHz */
        ADCSRA = _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
        ADCSRB = 0;

        for( uint8_t i=0; i<adcInputs; i++) {
          
            uint8_t adc = analogPins[i];
             
            /* Disable Digital input on ADC pins */
            if( adc < A8) {
                DIDR0 |= (1 << (adc - A0));
            } else {
                DIDR2 |= (1 << (adc - A8));
            }
        }

    }
#endif

    for( uint8_t i=0; i<switches; i++) {
        portsImpl->portInit( switchPins[i], INPUT);
    }
}

void InputImpl::start() {

#if defined(ARDUINO_ARCH_AVR)
    ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {
      
       mux = 0;
       setMux();

    }
#endif

}

void InputImpl::setMux() {

#if defined(ARDUINO_ARCH_AVR)
    uint8_t adc;

    if( mux < adcInputs) {

        adc = analogPins[mux];

        ADMUX &= ~(_BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0));
        
        if( adc < A8) {
          ADMUX |= (adc - A0);
          ADCSRB &= ~_BV(MUX5);
        } else {
          ADMUX |= (adc - A8);
          ADCSRB |= _BV(MUX5);
        }
          
        ADCSRA |= _BV(ADEN) | _BV(ADSC) | _BV(ADIE);
    }
#endif

}

switch_t InputImpl::GetSwitches() {

    return switches;
}
        
channelValue_t InputImpl::GetStickValue( channel_t ch) {

    if( ch < stickCount) {
      return GetAnalogValue( ch);
    }

    LOGV("InputImpl::GetStickValue: Illegal channel no. %d", ch);
    
    return 0;
}

channelValue_t InputImpl::GetTrimValue( channel_t ch) {

  if( ch < trimCount) {
      return GetAnalogValue( ch + stickCount);
  }

  LOGV("InputImpl::GetTrimValue: Illegal channel no. %d", ch);

  return 0;
}

channelValue_t InputImpl::GetAuxValue( channel_t ch) {

  if( ch < auxCount) {
      return GetAnalogValue( ch + stickCount + trimCount);
  }

  LOGV("InputImpl::GetAuxValue: Illegal channel no. %d", ch);

  return 0;
}

channelValue_t InputImpl::GetAnalogValue( channel_t ch) {

    channelValue_t v;

    if( ch < adcInputs) {
#if defined(ARDUINO_ARCH_AVR)
      ATOMIC_BLOCK( ATOMIC_RESTORESTATE) {
          v = adcValues[ch];
      }
#elif defined(ARDUINO_ARCH_ESP32)
# warning FIXIT
      v = 0;   
#endif
      return v;
    }

    LOGV("InputImpl::GetStickValue: Illegal channel no. %d", ch);

    return 0;
}

switchState_t InputImpl::GetSwitchValue( switch_t sw) {

    bool s1, s2;
    switchConf_t swConf;
    switchState_t state = SW_STATE_DONTCARE;

    if( sw >= switches) {
      LOGV("InputImpl::GetSwitchValue: Illegal switch no. %d", sw);
      return SW_STATE_DONTCARE;
    }
      
    swConf = switchConf[sw];
    
    switch( swConf) {

      case SW_CONF_2STATE:
        state = portsImpl->portGet(switchPins[sw]) ? SW_STATE_0 : SW_STATE_1;
        break;
        
      case SW_CONF_3STATE:
        portsImpl->portInit( switchPins[sw], INPUT);
        s1 = portsImpl->portGet(switchPins[sw]);
        portsImpl->portInit( switchPins[sw], INPUT_PULLUP);
        s2 = portsImpl->portGet(switchPins[sw]);

        if( !(s1 || s2)) state = SW_STATE_2;
        else if( s1 && s2) state = SW_STATE_0;
        else state = SW_STATE_1;
        break;
        
      case SW_CONF_UNUSED:
      case SW_CONF_CHANNEL:
      default:
        state = SW_STATE_DONTCARE;
    }

    return state;
}

switchConf_t InputImpl::GetSwitchConf( switch_t sw) {
  
  return switchConf[sw];
}
