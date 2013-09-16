ArduinoUNO_AudioCodecMikroe506 48kHz 16-bit Stereo PCM I/O for Arduino
======================================================================

Sep. 15th 2013

This is a library to drive the Mikroe 506 WM8731 Audio Codec Proto Board with ArduinoUNO.
It is a modified version of OpenMusicLabs AudioCodec library for use with the Mikroe 506 board in place of the OpenMusicLabs AudioCodec board. This library requires a small hardware mod on the Mikroe506 Proto Board, see instructions below.

Requirements
------------
Arduino UNO or similar (ATMega328P w/ 16Mhz internal clock)
Mikroe 506 Audio Codec Proto Board (with WM8731 CODEC and 12.288Mhz clock)

Instructions for Arduino UNO 
----------------------------

The following wiring implements both the I2C and SPI (Arduino SPI in Master Mode)
serial communication interfaces between the Arduino and the Mikroe506. 

ArduinoUNO    Mikroe506 Audio Proto
----------    ---------------------
D05           Timer input 1 from Modded WM8731 pin 2 via 220 Ohn resistor (see .jpg file and NOTE 1)
D10           ADCL + DACL output (parallel) via 220 Ohm resistor in series
D11           MOSI (master out, slave in)
D12           MISO (master in, slave out)
D13           SCK  (SPI clock)
(A0)          Optional potentiometer input - uncomment A0/A1 code to enable potentiometer inputs
(A1)          Optional potentiometer input
A4            SDA (I2C serial data)            
A5            SCL (I2C clock)
3.3V          3.3V
GND           GND

NOTES
-----
1. Connect CLKOUT (pin 2) from the WM831 chip to Timer T1 in (pin 5) on the Arduino UNO. 
Reason: the Mikroe 506 12.288MHz crystal output is needed to drive the DAC/ADC at the correct rate.
Consult the wiring diagram: ArduinoUno_Mikroe506AudioProto_labels.jpg

-mkc 9/15/13


