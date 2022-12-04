/*
esp32_pcnt.cpp is a wrapper for the hardware pulse counter devices in the Espressif ESP32 SoC.
This software is released to the public domain under the MIT Licence, copyright (c) 2022 Mike Gofton.
*/

#include <esp32_pcnt.h>

PulseCounter::PulseCounter() {
    
}

PulseCounter::~PulseCounter() {
    //free up assigned PCNT unit
    bitClear(counter_used,counter_id);
}

void PulseCounter::initialise(int PCNT_INPUT_SIG_IO, int PCNT_INPUT_CTRL_IO)
{
    // save channel and pin numbers
    sig_pin = PCNT_INPUT_SIG_IO;
    ctrl_pin = PCNT_INPUT_CTRL_IO;

    // allocate isr callback functions
    static void ((*isr_func[COUNTER_MAX])(void *)) = {
        unit0_isr, unit1_isr, unit2_isr, unit3_isr, unit4_isr, unit5_isr, unit6_isr, unit7_isr
    };

    //find a free pulse counter unit and allocate
    bool counter_allocated = false;
    for (uint8_t i = 0; i < COUNTER_MAX; i++) {
        // test which counter is free
        if (bitRead(counter_used,i) == 0) {
            counter_id = (pcnt_unit_t)i; // assign a free counter
            bitSet(counter_used,counter_id); // record in isr allocation register
            obj_instance[counter_id] = this; // record instance object reference
            counter_allocated = true;
            break;
        }
    }
    if (counter_allocated) {   
        // configure counter
        pcnt_config.unit = counter_id;
        pcnt_config.channel = PCNT_CHANNEL_0; //only use channel 0 of each counter unit.
        // Set signal and control inputGPIOs
        pcnt_config.pulse_gpio_num = sig_pin;
        pcnt_config.ctrl_gpio_num = ctrl_pin;
        pcnt_unit_config(&pcnt_config);
        //assign isr routine
        pcnt_isr_handler_add(counter_id, isr_func[counter_id], 0);     
    }
}

//definition of static class members used for handling of instance interrupts
uint8_t PulseCounter::counter_used = 0; //allocation register for counter use
PulseCounter* PulseCounter::obj_instance[COUNTER_MAX];


void PulseCounter::unit0_isr(void *) { PulseCounter::obj_instance[0]->instance_isr(); }
void PulseCounter::unit1_isr(void *) { PulseCounter::obj_instance[1]->instance_isr(); }
void PulseCounter::unit2_isr(void *) { PulseCounter::obj_instance[2]->instance_isr(); }
void PulseCounter::unit3_isr(void *) { PulseCounter::obj_instance[3]->instance_isr(); }
void PulseCounter::unit4_isr(void *) { PulseCounter::obj_instance[4]->instance_isr(); }
void PulseCounter::unit5_isr(void *) { PulseCounter::obj_instance[5]->instance_isr(); }
void PulseCounter::unit6_isr(void *) { PulseCounter::obj_instance[6]->instance_isr(); }
void PulseCounter::unit7_isr(void *) { PulseCounter::obj_instance[7]->instance_isr(); }

void PulseCounter::instance_isr() {
    // call the user isr
    usr_isr;
}
void PulseCounter::attach_interrupt(void (*isr_handler)(void *)) {
    // save user isr function
    usr_isr = isr_handler;
}


void PulseCounter::set_mode(pcnt_count_mode_t pos_mode, pcnt_count_mode_t neg_mode, pcnt_ctrl_mode_t hctrl_mode, pcnt_ctrl_mode_t lctrl_mode) {
    pcnt_set_mode(counter_id, PCNT_CHANNEL_0, pos_mode, neg_mode, hctrl_mode, lctrl_mode);
}

int16_t PulseCounter::get_value() {
    int16_t pulse_count;
    pcnt_get_counter_value(counter_id, &pulse_count);
    return pulse_count;
}

void PulseCounter::pause() {
    pcnt_counter_pause(counter_id);
}

void PulseCounter::resume() {
    pcnt_counter_resume(counter_id);
}

void PulseCounter::clear() {
    pcnt_counter_clear(counter_id);
}

void PulseCounter::interrupt_enable() {
    pcnt_intr_enable(counter_id);
}

void PulseCounter::interrupt_disable() {
    pcnt_intr_disable(counter_id);
}
/*
void PulseCounter::isr_register(void (*fn)(void *), void *arg) {
    pcnt_isr_register(fn, arg, (int)0, &isr_handle);
}

void PulseCounter::isr_unregister() {
    esp_intr_free(isr_handle);
}
*/
void PulseCounter::set_filter_value(uint16_t filter_val) {
    pcnt_set_filter_value(counter_id, filter_val);
    pcnt_filter_enable(counter_id);
}

void PulseCounter::filter_enable() {
    pcnt_filter_enable(counter_id);
}

void PulseCounter::filter_disable() {
    pcnt_filter_disable(counter_id);
}

void PulseCounter::event_enable(pcnt_evt_type_t evt_type) {
    pcnt_event_enable(counter_id, evt_type);
}

void PulseCounter::event_disable(pcnt_evt_type_t evt_type) {
    pcnt_event_disable(counter_id, evt_type);
}

void PulseCounter::set_event_value(pcnt_evt_type_t evt_type, int16_t value) {
    pcnt_set_event_value(counter_id, evt_type, value);
    pcnt_event_enable(counter_id, evt_type);
}

void PulseCounter::get_event_value(pcnt_evt_type_t evt_type, int16_t *value) {
    pcnt_get_event_value(counter_id, evt_type, value);
}

uint8_t PulseCounter::event_status() {
    uint32_t status;
    pcnt_get_event_status(counter_id, &status);
    return (uint8_t)(status);
}