/*
mic_transmitter.ino
mcasey - Bregman Studio, Dartmouth College, 9.30.13

This program reads audio data from the Mikroe506 board's microphone input.
It relays the input back to the DACs for listening and to the UART TX for transmission.
*/

// setup codec parameters
// must be done before #includes
// see readme file in libraries folder for explanations
#define SAMPLE_RATE 8 // 8kHz (WM8731@12.288MHz, Arduino@16MHz) ) mkc 9/15/13
#define ADCS 0 // use no ADCs (potentiometers)

// WM8731 Microphone control
#define MUTEMIC 0 // disable MUTEMIC
#define INSEL 1 // enable MIC input
#define MICBOOST 1 // enable MICBOOST
#define BYPASS 1 // Bypass LINEIN
#define DACSEL 1 // Select DAC for audio loopback
#define SIDETONE 0 // Deselect SIDETONE
#define SIDEATT 0

// include necessary libraries
#include <Wire.h>
#include <SPI.h>
#include <AudioCodec.h>

// create data variables for audio transfer
// even though there is no input needed, the codec requires stereo data
int left_in = 0; // in from codec (LINE_IN)
int right_in = 0;
int left_out = 0; // out to codec (HP_OUT)
int right_out = 0;

void setup() {
  // call this last if you are setting up other things
  AudioCodec_init(); // setup codec and microcontroller registers
  Serial.begin(115200);
}

void loop() {
  while (1); // reduces clock jitter
}

// timer1 interrupt routine - all data processed here
ISR(TIMER1_COMPA_vect, ISR_NAKED) { // dont store any registers
  // &'s are necessary on data_in variables
  AudioCodec_data(&left_in, &right_in, left_out, right_out);
  Serial.write((char)(left_out>>8)); // Write high 8 bits as char
  
  // our sinewave is now in temp2
  left_out = right_in; // put incoming audio on left channel
  right_out = right_in; // put sinusoid out on right channel

  reti(); // dont forget to return from the interrupt
}
