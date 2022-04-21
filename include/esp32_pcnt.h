/*
esp32_pcnt.h is a wrapper for the hardware pulse counter devices in the Espressif ESP32 SoC.

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

#define COUNTER_MAX 8 // maximum number of counter instances

#include <Arduino.h>
#include <driver/pcnt.h>

enum {
    evt_thres1 = 1<<2,
    evt_thres0 = 1<<3,
    evt_low_lim = 1<<4,
    evt_high_lim = 1<<5,
    evt_zero = 1<<6
    };

class PulseCounter
{
    public:
        PulseCounter();
        ~PulseCounter();
        void initialise(int PCNT_INPUT_SIG_IO, int PCNT_INPUT_CTRL_IO);
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
        void attach_interrupt(void (*isr_handler)(uint8_t event));

    private:
        int16_t sig_pin, ctrl_pin;
        pcnt_count_mode_t pos_mode , neg_mode;
        pcnt_ctrl_mode_t hctrl_mode , lctrl_mode;
        pcnt_isr_handle_t isr_handle;
        pcnt_config_t pcnt_config;
        pcnt_unit_t counter_id; // (0 ~ 7) for ESP32
        void instance_isr();
        void (*usr_isr) (uint8_t event);
        static uint8_t counter_used; // record counter allocation with a bit mask, i.e. bit 0 = counter 0, bit 1 = counter 1 
        static PulseCounter* obj_instance[COUNTER_MAX]; // array to hold instance object references
        static void unit0_isr(void *);
        static void unit1_isr(void *);
        static void unit2_isr(void *);
        static void unit3_isr(void *);
        static void unit4_isr(void *);
        static void unit5_isr(void *);
        static void unit6_isr(void *);
        static void unit7_isr(void *);
};
#endif
