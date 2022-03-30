# ESP32PulseCounter
Arduino library for the 8 x 16 bit hardware counters available on the ESP32

The PCNT (Pulse Counter) module is designed to count the number of rising and/or falling edges of an input signal.
Each pulse counter unit has a 16-bit signed counter register and two channels which can be configured
to either increment or decrement the counter.
Each channel has a signal input that accepts signal edges to be detected, as well as a control input which can be used 
to enable or disable the signal input.
The count and control inputs have optional filters that can be used to discard unwanted glitches in the signal.
There are five counter state watch events which are able to trigger an interrupt. The event happens on the pulse counter
reaching specific values: minimum or maximum count values, two threshold values and counter = zero.

This library is coded for the Arduino Framework and is based on ESP-IDF release 4.4

ESP-IDF pulse counter documentation is available here :
https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32/api-reference/peripherals/pcnt.html

A detailed description of the pulse counter hardware is available in Chapter 17 of the Technical Reference Manual :
https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
