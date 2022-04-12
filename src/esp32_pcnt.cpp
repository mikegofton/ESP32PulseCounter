/*
esp32_pcnt.cpp is a wrapper for the hardware pulse counter devices in the Espressif ESP32 SoC.
This software is released to the public domain under the MIT Licence, copyright (c) 2022 Mike Gofton.
*/

#include <esp32_pcnt.h>


void PulseCounter::initialise(uint8_t PCNT_UNIT, uint8_t PCNT_CHANNEL, int PCNT_INPUT_SIG_IO, int PCNT_INPUT_CTRL_IO = 0)
{
    // save unit, channel and pin numbers
    unit = (pcnt_unit_t)PCNT_UNIT;
    channel = (pcnt_channel_t)PCNT_CHANNEL;
    sig_pin = PCNT_INPUT_SIG_IO;
    ctrl_pin = PCNT_INPUT_CTRL_IO;
    // configure counter
        // Configure a PCNT unit and channel
        pcnt_config.channel = channel;
        pcnt_config.unit = unit;
        // Set PCNT input signal and control GPIOs
        pcnt_config.pulse_gpio_num = PCNT_INPUT_SIG_IO;
        if (PCNT_INPUT_CTRL_IO == 0) {
            pcnt_config.ctrl_gpio_num = PCNT_PIN_NOT_USED;
        } else {
            pcnt_config.ctrl_gpio_num = PCNT_INPUT_CTRL_IO;
        }
    pcnt_unit_config(&pcnt_config);
}

void PulseCounter::set_mode(pcnt_count_mode_t pos_mode, pcnt_count_mode_t neg_mode, pcnt_ctrl_mode_t hctrl_mode, pcnt_ctrl_mode_t lctrl_mode)
{
    pcnt_set_mode(unit, channel, pos_mode, neg_mode, hctrl_mode, lctrl_mode);
}

int16_t PulseCounter::get_value()
{
    int16_t pulse_count;
    pcnt_get_counter_value(unit, &pulse_count);
    return pulse_count;
}

void PulseCounter::pause()
{
    pcnt_counter_pause(unit);
}

void PulseCounter::resume()
{
    pcnt_counter_resume(unit);
}

void PulseCounter::clear()
{
    pcnt_counter_clear(unit);
}

void PulseCounter::interrupt_enable()
{
    pcnt_intr_enable(unit);
}

void PulseCounter::interrupt_disable()
{
    pcnt_intr_disable(unit);
}

void PulseCounter::isr_register(void (*fn)(void *), void *arg)
{
    pcnt_isr_register(fn, arg, (int)0, &isr_handle);
}

void PulseCounter::isr_unregister()
{
    esp_intr_free(isr_handle);
}

void PulseCounter::set_filter_value(uint16_t filter_val)
{
    pcnt_set_filter_value(unit, filter_val);
    pcnt_filter_enable(unit);
}

void PulseCounter::filter_enable()
{
    pcnt_filter_enable(unit);
}

void PulseCounter::filter_disable()
{
    pcnt_filter_disable(unit);
}

void PulseCounter::event_enable(pcnt_evt_type_t evt_type)
{
    pcnt_event_enable(unit, evt_type);
}

void PulseCounter::event_disable(pcnt_evt_type_t evt_type)
{
    pcnt_event_disable(unit, evt_type);
}

void PulseCounter::set_event_value(pcnt_evt_type_t evt_type, int16_t value)
{
    pcnt_set_event_value(unit, evt_type, value);
    pcnt_event_enable(unit, evt_type);
}

void PulseCounter::get_event_value(pcnt_evt_type_t evt_type, int16_t *value)
{
    pcnt_get_event_value(unit, evt_type, value);
}