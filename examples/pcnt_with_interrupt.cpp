#include <Arduino.h>
#include <esp32_pcnt.h>

/*
Program to demonstrate use of an ESP32 hardware timer with interrupts
*/

const uint8_t input_pin = 22; // GPIO pin for counter signal input
const uint8_t pcnt_unit = 0; // counter unit
const uint8_t pcnt_channel = 0; // counter channel
int16_t threshold_0 = 10;
int16_t threshold_1 = 20;
volatile bool interrupt_flag = false; // interrupt flag
volatile uint32_t unit_int; //counter units generating an interrupt

//create counter object
PulseCounter pc0;

//pulse counter ISR
void IRAM_ATTR pcnt_intr_handler(void *arg) {
    // identify the counter unit raising the interrupt.
    // Bit 0-7 being set corresponds to unit 0-7 generating the interrupt.
    unit_int = PCNT.int_st.val;
    // set a flag to capture the interrupt
    interrupt_flag = true;
    // clear the specific counter unit interrupt flags. A single PCNT interrupt handler is used for all counter units.
    PCNT.int_clr.val = unit_int;
}

void setup() {
  // setup hardware pulse counter

  // initialise counter unit 0, channel 0 with signal input GPIO pin and control signal input pin (0 = no control signal input)
  pc0.initialise(pcnt_unit,pcnt_channel,input_pin,0);
  // count up on negative edges, don't count on positive edges 
  pc0.set_mode(PCNT_COUNT_DIS,PCNT_COUNT_INC,PCNT_MODE_KEEP,PCNT_MODE_KEEP);
  // set glich filter to ignore pulses less than 100 x 2.5ns
  pc0.set_filter_value(100); 
  // set and enable threshold 0 and 1 watch points - these will trigger an interrupt.
  // the event can be disabled and enabled using pc0.event_disable(PCNT_EVT_THRES_0) or pc0.event_enable(PCNT_EVT_THRES_0)
  // thereshold 0 & 1 events do not stop the counter
  pc0.set_event_value(PCNT_EVT_THRES_0,threshold_0);
  pc0.set_event_value(PCNT_EVT_THRES_1,threshold_1);
  // register interrupt service routine for all counter units, and pass an optional argument (NULL in this case)
  pc0.isr_register(pcnt_intr_handler, NULL);
  pc0.interrupt_enable(); //
  // clear and restart the counter
  pc0.clear();
  pc0.resume();
}

void loop() {

  if (interrupt_flag) {
    interrupt_flag = false;
    // code to action the interrupt for the units identified in unit_int
    // Bit 0-7 being set corresponds to unit 0-7 generating the interrupt.
  }

}