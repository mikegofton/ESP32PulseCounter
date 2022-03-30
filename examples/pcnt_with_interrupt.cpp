#include <Arduino.h>
#include <esp32_pcnt.h>

/*
Program to demonstrate use of an ESP32 hardware timer with interrupts
*/

const uint8_t input_pin = 22; //GPIO pin for ASK signal
const uint8_t pcnt_unit = 0; //hardware pulse counter unit used
const uint8_t pcnt_channel = 0; //hardware pulse counter channel used
volatile bool start_pulse = false; //flag for start bit of received character

//create counter object
PulseCounter pc0;

//mutex for pulse counter
portMUX_TYPE pcntMux = portMUX_INITIALIZER_UNLOCKED;

//pulse counter ISR - used to initiate the bit clock interrupts for ASK demodulation
void IRAM_ATTR pcnt_intr_handler(void *arg) {
  portENTER_CRITICAL_ISR(&pcntMux);
  start_pulse = true;
  PCNT.int_clr.val = bit(pcnt_unit);// need to clear the specific counter unit interrupt flag. A single PCNT interrupt handler is used.
  portEXIT_CRITICAL_ISR(&pcntMux);
}

void setup() {
  //setup serial comms
  Serial.begin(115200,SERIAL_8N1);//terminal to monitor function

  // setup hardware pulse counter
  pc0.initialise(pcnt_unit,pcnt_channel,input_pin,0); // use unit 0, channel 0, signal input on demod_in GPIO, no control signal input
  pc0.set_mode(PCNT_COUNT_DIS,PCNT_COUNT_INC,PCNT_MODE_KEEP,PCNT_MODE_KEEP);//count up on negative edges, don't count on positive edges
  pc0.set_filter_value(100); //set glich filter to ignore pulses less than 100 x 2.5ns
  pc0.set_event_value(PCNT_EVT_THRES_0,1); //set threshold 0 watch point to count = 1, i.e. first negative going pulse,used to synchronise bit reception
  pc0.isr_register(pcnt_intr_handler, NULL, 0);
  pc0.interrupt_enable();
  pc0.clear();
  pc0.resume();
}

void loop() {
  if (start_pulse) {
    start_pulse = false;
    pc0.event_disable(PCNT_EVT_THRES_0);// disable further pcnt threshold events (interrupts)- the pulse counter keeps going
  }

}
