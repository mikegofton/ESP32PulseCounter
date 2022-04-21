#include <Arduino.h>
#include <esp32_pcnt.h>

/*
Program to demonstrate use of an ESP32 hardware timer with interrupts
*/

const int input_pin0 = 22;
const int input_pin1 = 21; // GPIO pin for counter signal input
int16_t threshold_0 = 1000;
int16_t threshold_1 = 2000;
int16_t high_limit = 3000;
volatile bool pc0_int_flag = false; // interrupt flag
volatile bool pc1_int_flag = false; // interrupt flag
volatile uint8_t pc0_event;
volatile uint8_t pc1_event;

//create counter object
PulseCounter pc0,pc1;

//mutex for hardware pulse counters
portMUX_TYPE pcntMux0 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE pcntMux1 = portMUX_INITIALIZER_UNLOCKED;

//pulse counter ISR
void IRAM_ATTR pc0_isr(uint8_t event) {
  // Prevent context switching during the interrupt service routine with an ISR spinlock
  portENTER_CRITICAL_ISR(&pcntMux0); 
    // set interrupt flag
    pc0_int_flag = true;
    // get events
    pc0_event = event;
  portEXIT_CRITICAL_ISR(&pcntMux0);
}
//pulse counter ISR
void IRAM_ATTR pc1_isr(uint8_t event) {
  // Prevent context switching during the interrupt service routine with an ISR spinlock
  portENTER_CRITICAL_ISR(&pcntMux1);
    // set interrupt flag
    pc1_int_flag = true;
    // get events
    pc1_event = event; 
  portEXIT_CRITICAL_ISR(&pcntMux1);
}

void setup() {

  Serial.begin(115200,SERIAL_8N1);

  // setup hardware pulse counter

  // initialise counter unit 0, channel 0 with signal input GPIO pin and control signal input pin (0 = no control signal input)
  pc0.initialise(input_pin0,PCNT_PIN_NOT_USED);
  pc1.initialise(input_pin1,PCNT_PIN_NOT_USED);
  // count up on negative edges, don't count on positive edges 
  pc0.set_mode(PCNT_COUNT_DIS,PCNT_COUNT_INC,PCNT_MODE_KEEP,PCNT_MODE_KEEP);
  pc1.set_mode(PCNT_COUNT_DIS,PCNT_COUNT_INC,PCNT_MODE_KEEP,PCNT_MODE_KEEP);
  // set glich filter to ignore pulses less than 100 x 2.5ns
  pc0.set_filter_value(100);
  pc1.set_filter_value(100); 
  // set and enable threshold 0 and 1 watch points - these will trigger an interrupt.
  // the event can be disabled and enabled using pc0.event_disable(PCNT_EVT_THRES_0) or pc0.event_enable(PCNT_EVT_THRES_0)
  // thereshold 0 & 1 events do not stop the counter
  pc0.set_event_value(PCNT_EVT_THRES_0,threshold_0);
  pc1.set_event_value(PCNT_EVT_THRES_0,threshold_0);
  pc0.set_event_value(PCNT_EVT_THRES_1,threshold_1);
  pc1.set_event_value(PCNT_EVT_THRES_1,threshold_1);
  pc0.set_event_value(PCNT_EVT_H_LIM,high_limit);
  pc1.set_event_value(PCNT_EVT_H_LIM,high_limit);
  // register interrupt service routine for all counter units, and pass an optional argument (NULL in this case)
  // pc0.isr_register(pcnt_intr_handler, NULL);
  pc0.attach_interrupt(pc0_isr);
  pc0.interrupt_enable(); //
  pc1.attach_interrupt(pc1_isr);
  pc1.interrupt_enable(); //

  // clear and restart the counter
  pc0.clear();
  pc0.resume();
  pc1.clear();
  pc1.resume();
}

void loop() {
  
  if (pc0_int_flag) {
    pc0_int_flag = false;
    Serial.print("pc0 : ");
    switch (pc0_event)
    {
      case evt_thres0:
        Serial.println("Threshold 0");
        break;
      case evt_thres1:
        Serial.println("Threshold 1");;
        break;
      case evt_high_lim:
        Serial.println("High Limit");;
        break;
    }
  }
  if (pc1_int_flag) {
    pc1_int_flag = false;
    Serial.print("pc1 : ");
    switch (pc1_event)
    {
      case evt_thres0:
        Serial.println("Threshold 0");
        break;
      case evt_thres1:
        Serial.println("Threshold 1");;
        break;
      case evt_high_lim:
        Serial.println("High Limit");;
        break;
    }
  }
}
