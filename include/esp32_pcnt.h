/*
esp32_pcnt.h is a wrapper for the hardware pulse counter devices in the Espressif ESP32 SoC

The PCNT (Pulse Counter) module is designed to count the number of rising and/or falling edges of an input signal.
Each pulse counter unit has a 16-bit signed counter register and two channels which can be configured
to either increment or decrement the counter.
Each channel has a signal input that accepts signal edges to be detected, as well as a control input which can be used 
to enable or disable the signal input.
The count and control inputs have optional filters that can be used to discard unwanted glitches in the signal.
There are five counter state watch events which are able to trigger an interrupt. The event happens on the pulse counter
reaching specific values: minimum or maximum count values, two threshold values and counter = zero.

This library is coded for the Arduino Framework and is based on ESP-IDF relese 4.4
Additional ESP=IDF documentation is available here 
https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32/api-reference/peripherals/pcnt.html

This software is released to the public domain under the MIT Licence, copyright (c) 2022 Mike Gofton.
*/

#ifndef PulseCounter_h
#define PulseCounter_h

#include <Arduino.h>
#include <driver/pcnt.h>

class PulseCounter
{
    public:
        void initialise(uint8_t PCNT_UNIT, uint8_t PCNT_CHANNEL, int PCNT_INPUT_SIG_IO, int PCNT_INPUT_CTRL_IO);
        void set_mode(pcnt_count_mode_t pos_mode, pcnt_count_mode_t neg_mode, pcnt_ctrl_mode_t hctrl_mode, pcnt_ctrl_mode_t lctrl_mode);
        int16_t get_value();
        void pause();
        void resume();
        void clear();
        void interrupt_enable();
        void interrupt_disable();
        void event_enable(pcnt_evt_type_t evt_type);
        void event_disable(pcnt_evt_type_t evt_type);
        void set_event_value(pcnt_evt_type_t evt_type, int16_t value);
        void get_event_value(pcnt_evt_type_t evt_type, int16_t *value);
        void isr_register(void (*fn)(void *), void *arg);
        void isr_unregister();
        void set_filter_value(uint16_t filter_val);
        void filter_enable();
        void filter_disable();
    private:
        pcnt_unit_t unit;
        pcnt_channel_t channel;
        int16_t sig_pin , ctrl_pin ;
        pcnt_count_mode_t pos_mode , neg_mode;
        pcnt_ctrl_mode_t hctrl_mode , lctrl_mode;
        pcnt_isr_handle_t isr_handle;
        pcnt_config_t pcnt_config;
};
#endif