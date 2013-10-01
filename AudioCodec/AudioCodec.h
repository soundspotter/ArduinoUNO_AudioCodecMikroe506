// AudioCodec.h
// mkc (soundspotter) - updated for Arduino UNO with Mikroe 506 WM8731 CODEC
//
// guest openmusiclabs 7.28.11
// this is the library file for ARDUINO -> there is a different
// file for Maple, make sure you are using the right one.
// place this file in the libraries file of your Arduino sketches folder
// e.g. C:\Documents and Settings\user\My Documents\Arduino\libraries\
// you may have to create the \libraries folder
//
// updated for arduino1.0 - i2c commands changed, and "Arduino.h" added 1.9.12


#ifndef AudioCodec_h // include guard
#define AudioCodec_h

//#include "WProgram.h"
#include "Arduino.h"
#include <avr/pgmspace.h>
#include "mult16x16.h"
#include "mult16x8.h"
#include "mult32x16.h"


#ifndef SAMPLE_RATE
  #define SAMPLE_RATE 44
#elif (SAMPLE_RATE == 88)||(SAMPLE_RATE == 48)||(SAMPLE_RATE == 44)||(SAMPLE_RATE == 22)||(SAMPLE_RATE == 8)||(SAMPLE_RATE == 2)
#else
  #error SAMPLE_RATE value not defined
#endif

#ifndef ADCHPD
  #define ADCHPD 0
#elif (ADCHPD == 0)||(ADCHPD == 1)
#else
  #error ADCHPD value not defined
#endif

#ifndef ADCS
  #define ADCS 2
#elif (ADCS >=0)&&(ADCS <= 2)
#else
  #error ADCS value not defined
#endif

#ifndef HYST
  #define HYST 32
#elif (HYST >= 0)&&(HYST <= 255)
#else
  #error HYST value not defined
#endif

#ifndef LINVOL
  #define LINVOL 23
#elif (LINVOL >= 0) && (LINVOL <= 0x1f)
#else
  #error LINVOL value not defined
#endif

#ifndef RINVOL
  #define RINVOL 23
#elif (RINVOL >= 0) && (RINVOL <= 0x1f)
#else
  #error RINVOL value not defined
#endif

#ifndef LHPVOL
  #define LHPVOL 121
#elif (LHPVOL == 0) || ((LHPVOL >= 0x30) && (LHPVOL <= 0x7f))
#else
  #error LHPVOL value not defined
#endif

#ifndef RHPVOL
  #define RHPVOL 121
#elif (RHPVOL == 0) || ((RHPVOL >= 0x30) && (RHPVOL <= 0x7f))
#else
  #error RHPVOL value not defined
#endif

#ifndef MICBOOST
  #define MICBOOST 0
#elif (MICBOOST == 0)||(MICBOOST == 1)
#else
  #error MICBOOST value not defined
#endif

#ifndef MUTEMIC
  #define MUTEMIC 1
#elif (MUTEMIC == 0)||(MUTEMIC == 1)
#else
  #error MUTEMIC value not defined
#endif

#ifndef INSEL
  #define INSEL 0
#elif (INSEL == 0)||(INSEL == 1)
#else
  #error INSEL value not defined
#endif

#ifndef BYPASS
  #define BYPASS 0
#elif (BYPASS == 0)||(BYPASS == 1)
#else
  #error BYPASS value not defined
#endif

#ifndef DACSEL
  #define DACSEL 1
#elif (DACSEL == 0)||(DACSEL == 1)
#else
  #error DACSEL value not defined
#endif

#ifndef SIDETONE
  #define SIDETONE 0
#elif (SIDETONE == 0)||(SIDETONE == 1)
#else
  #error SIDETONE value not defined
#endif

#ifndef SIDEATT
  #define SIDEATT 0
#elif (SIDEATT >= 0)&&(SIDEATT <= 3)
#else
  #error SIDEATT value not defined
#endif

// setup variables for ADC
#if ADCS == 0
  // do nothing
#elif ADCS == 1
  unsigned char _i = 64;
  unsigned int _mod0temp = 0x0000;
#elif ADCS == 2
  unsigned char _i = 130;
  unsigned int _mod0temp = 0x0000;
  unsigned int _mod1temp = 0x0000;
#endif


static inline void AudioCodec_init(void) {

  // setup spi peripheral
  digitalWrite(10, LOW); // set ss pin to output low
  pinMode (10, OUTPUT);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);
  
  // setup i2c pins and configure codec
  // the new Wire library has trouble with 0x00, so (uint8_t) is added
  Wire.begin();
  Wire.beginTransmission(0x1a);
  Wire.write(0x0c); // power reduction register
  Wire.write((uint8_t)0x00); // turn everything on
  Wire.endTransmission();
  
  Wire.beginTransmission(0x1a);
  Wire.write(0x0e); // digital data format
  Wire.write(0x03); // 16b SPI mode
  Wire.endTransmission();
  
  Wire.beginTransmission(0x1a);
  Wire.write((uint8_t)0x00); // left in setup register
  Wire.write((uint8_t)LINVOL);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x1a);
  Wire.write(0x02); // right in setup register
  Wire.write((uint8_t)RINVOL);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x1a);
  Wire.write(0x04); // left headphone out register
  Wire.write((uint8_t)LHPVOL);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x1a);
  Wire.write(0x06); // right headphone out register
  Wire.write((uint8_t)RHPVOL);
  Wire.endTransmission();

  Wire.beginTransmission(0x1a);
  Wire.write(0x0a); // digital audio path configuration
  Wire.write((uint8_t)ADCHPD);
  Wire.endTransmission();
  
  Wire.beginTransmission(0x1a);
  Wire.write(0x08); // analog audio pathway configuration
  Wire.write((uint8_t)((SIDEATT << 6)|(SIDETONE << 5)|(DACSEL << 4)|(BYPASS << 3)|(INSEL << 2)|(MUTEMIC << 1)|(MICBOOST << 0)));
  Wire.endTransmission();
  
  Wire.beginTransmission(0x1a);
  Wire.write(0x10); // WM8731 CORE CLOCK config for ATMega328p internal clock w/ SPI_CLKDIV1 (16MHz)
  #if SAMPLE_RATE == 88
    USBNORMAL=0;BOSR=1;SR0=1;SR1=1;SR2=1;SR3=1;CLKDIV=0;CLKODIV=0;
  #elif SAMPLE_RATE == 48
    USBNORMAL=0;BOSR=0;SR0=0;SR1=0;SR2=0;SR3=0;CLKDIV=0;CLKODIV=0;
  #elif SAMPLE_RATE == 44
    USBNORMAL=0;BOSR=0;SR0=1;SR1=0;SR2=1;SR3=1;CLKDIV=0;CLKODIV=0;
  #elif SAMPLE_RATE == 22
    USBNORMAL=0;BOSR=0;SR0=1;SR1=0;SR2=1;SR3=1;CLKDIV=0;CLKODIV=0;
  #elif SAMPLE_RATE == 8
    USBNORMAL=0;BOSR=0;SR0=1;SR1=1;SR2=0;SR3=0;CLKDIV=0;CLKODIV=0;
  #elif SAMPLE_RATE == 2
    USBNORMAL=0;BOSR=1;SR0=1;SR1=1;SR2=0;SR3=0;CLKDIV=0;CLKODIV=0;
  #endif
  Wire.write((uint8_t)((CLKODIV2<<7)|(CLKODIV<<6)|(SR3<<5)|(SR2<<4)|(SR1<<3)|(SR0<<2)|(BOSR<<1)|(USBNORMAL<<0)));
  Wire.endTransmission();

  Wire.beginTransmission(0x1a);
  Wire.write(0xc); // LININ, CLKOUT power down
  Wire.write(0x41);
  Wire.endTransmission();

  Wire.beginTransmission(0x1a);
  Wire.write(0x12); // codec enable
  Wire.write(0x01);
  Wire.endTransmission();
  
  // setup ADCs
  #if ADCS == 0
    DIDR0 = (1 << ADC1D)|(1 << ADC0D); // turn off digital inputs for ADC0 and ADC1
  #elif (ADCS == 1) || (ADCS == 2)
    ADMUX = 0x40; // start with ADC0 - internal VCC for Vref
    ADCSRA = 0xe7; // ADC enable, autotrigger, ck/128
    ADCSRB = 0x00; // free running mode
    DIDR0 = (1 << ADC1D)|(1 << ADC0D); // turn off digital inputs for ADC0 and ADC1
  #endif
  
  // setup timer1 for codec clock division
  TCCR1A = 0x00; // set to CTC mode
  TCCR1B = 0x0a; // set to CTC and Timer1 prescaler DIV8, 0xa (internal clock 16MHz/8
  TCCR1C = 0x00; // not used
  TCNT1H = 0x00; // clear the counter
  TCNT1H = 0x00;
  #if SAMPLE_RATE == 88
    OCR1AH = 0x00; // set the counter top for 16MHz internal clock
    OCR1AL = 0x17;
  #elif (SAMPLE_RATE == 48) 
    OCR1AH = 0x00; // set the counter top
    OCR1AL = 0x2a;    
  #elif (SAMPLE_RATE == 44) || (SAMPLE_RATE == 22)
    OCR1AH = 0x00; // set the counter top
    OCR1AL = 0x2d; 
  #elif SAMPLE_RATE == 8
    OCR1AH = 0x00; // set the counter top
    OCR1AL = 0xfa; //
  #elif SAMPLE_RATE == 2
    OCR1AH = 0x03; // set the counter top
    OCR1AL = 0xe8;
  #endif
    //  TIMSK1 = 0x02; // turn on compare match interrupt // ** SHOULD THIS BE 0x4, MKC 9/15/13 ??
  TIMSK1 |= (1 << OCIE1A); // turn on compare match interrupt // ** SHOULD THIS BE 0x4, MKC 9/15/13 ??
  
  // turn off all enabled interrupts (delay and wire)
  TIMSK0 = 0x00;
  TWCR = 0x00;

  sei(); // turn on interrupts
}


// adc sample routine
// this creates relatively low noise 16b values from adc samples
#if ADCS == 0
  static inline void AudioCodec_ADC() {
    // do nothing
  }
#elif ADCS == 1
  static inline void AudioCodec_ADC(unsigned int* _mod0value) {
    if (ADCSRA & (1 << ADIF)) { // check if sample ready
      _mod0temp += ADCL; // fetch ADCL first to freeze sample
      _mod0temp += (ADCH << 8); // add to temp register
      ADCSRA = 0xf7; // reset the interrupt flag
      if (--_i == 0) { // check if enough samples have been averaged
        // add in hysteresis to remove jitter
        if (((_mod0temp - *_mod0value) < HYST) || ((*_mod0value - _mod0temp) < HYST)) {
        }
        else {
          *_mod0value = _mod0temp; // move temp value
	}
        _mod0temp = 0x0000; // reset temp value
        _i = 64; // reset counter
      }
    }
  }
#elif ADCS == 2
  static inline void AudioCodec_ADC(unsigned int* _mod0value, unsigned int* _mod1value) {
    if (ADCSRA & (1 << ADIF)) { // check if sample ready
      --_i; // check which sample we are on
      if (_i == 129) { // do nothing, first sample after mux change
      }
      else if (_i >= 65) { // sample ADC0
        _mod0temp += ADCL; // fetch ADCL first to freeze sample
        _mod0temp += (ADCH << 8); // add to temp register
        if (_i == 65) { // check if enough samples have been averaged
          // add in hysteresis to remove jitter
          if (((_mod0temp - *_mod0value) < HYST) || ((*_mod0value - _mod0temp) < HYST)) {
          }
          else {
            *_mod0value = _mod0temp; // move temp value
	    }
          _mod0temp = 0x0000; // reset temp value
          ADMUX = 0x41; // switch to ADC1
        }
      }
      else if (_i < 64) { // sample ADC1, first sample (64) after mux change ignored
        _mod1temp += ADCL; // fetch ADCL first to freeze sample
        _mod1temp += (ADCH << 8); // add to temp register
        if (_i == 0) { // check if enough samples have been averaged
          // add in hysteresis to remove jitter
          if (((_mod1temp - *_mod1value) < HYST) || ((*_mod1value - _mod1temp) < HYST)) {
          }
          else {
            *_mod1value = _mod1temp; // move temp value
	    }
          _mod1temp = 0x0000; // reset temp value
          ADMUX = 0x40; // switch to ADC0
          _i = 130; // reset counter
        }
      }
      ADCSRA = 0xf7; // reset the interrupt flag
    }
  }
#endif


// codec data transfer function
static inline void AudioCodec_data(int* _lin, int* _rin, int _lout, int _rout) {

  int _out_temp = _lout;
  PORTB |= (1<<PORTB2);  // toggle ss pin // PORTB2 IS PIN 10
  asm volatile ("out %0, %B1" : : "I" (_SFR_IO_ADDR(SPDR)), "r" (_out_temp) );
  PORTB &= ~(1<<PORTB2); // toggle ss pin
  while(!(SPSR & (1<<SPIF))){ // wait for data transfer to complete
  }
  asm volatile ("out %0, %A1" : : "I" (_SFR_IO_ADDR(SPDR)), "r" (_out_temp) );
  asm volatile ("in r3, %0" : : "I" (_SFR_IO_ADDR(SPDR)) );
  _out_temp = _rout;
  while(!(SPSR & (1<<SPIF))){ // wait for data transfer to complete
  }
  asm volatile ("out %0, %B1" : : "I" (_SFR_IO_ADDR(SPDR)), "r" (_out_temp) );
  asm volatile ("in r2, %0" : : "I" (_SFR_IO_ADDR(SPDR)) );
  asm volatile ("movw %0, r2" : "=r" (*_lin) : );

  while(!(SPSR & (1<<SPIF))){ // wait for data transfer to complete
  }
  asm volatile ("out %0, %A1" : : "I" (_SFR_IO_ADDR(SPDR)), "r" (_out_temp) );
  asm volatile ("in r3, %0" : : "I" (_SFR_IO_ADDR(SPDR)) );
  while(!(SPSR & (1<<SPIF))){ // wait for data transfer to complete
  }
  asm volatile ("in r2, %0" : : "I" (_SFR_IO_ADDR(SPDR)) );
  asm volatile ("movw %0, r2" : "=r" (*_rin) : );
}


#endif // end include guard

