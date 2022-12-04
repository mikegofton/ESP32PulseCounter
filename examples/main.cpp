#include <Arduino.h>
#include <esp32_pcnt.h>
#include <Ticker.h>

/*
Program to demonstrate use of ESP32 hardware timers implementing threshold interrupts.
Clock signals are generated on two GPIO pins (23 and 19) - these are connected to hardware counters 0 and 1 via GPIO pins 22 and 21 respectively.
*/


#define output_pin0 23 // GPIO pin for clock signal 0
#define output_pin1 19 // GPIO pin for clock signal 1
#define input_pin0 22 // GPIO pin for counter 0 input
#define input_pin1 21 // GPIO pin for counter 1 input

#define threshold_0 10
#define threshold_1 20
#define high_limit 30

#define clock_duration 500 // one half the signal output duration in millisecs

/*
int16_t threshold_0 = 1000;
int16_t threshold_1 = 2000;
int16_t high_limit = 3000;
*/
volatile bool pc0_int_flag = false; // counter 0 interrupt flag
volatile bool pc1_int_flag = false; // counter 1 interrupt flag
volatile bool clock_int_flag = false; // clock signal interrupt flag

//create counter object
PulseCounter pc0,pc1;
Ticker t0;

//mutex for hardware pulse counters
portMUX_TYPE pcntMux0 = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE pcntMux1 = portMUX_INITIALIZER_UNLOCKED;

//pulse counter ISR
IRAM_ATTR void pc0_isr(void *) {
  // Prevent context switching during the interrupt service routine with an ISR spinlock
  portENTER_CRITICAL_ISR(&pcntMux0); 
    // set interrupt flag
    pc0_int_flag = true;
    // get events
  portEXIT_CRITICAL_ISR(&pcntMux0);
}
//pulse counter ISR
IRAM_ATTR void pc1_isr(void *) {
  // Prevent context switching during the interrupt service routine with an ISR spinlock
  portENTER_CRITICAL_ISR(&pcntMux1);
    // set interrupt flag
    pc1_int_flag = true;
    // get events
  portEXIT_CRITICAL_ISR(&pcntMux1);
}

IRAM_ATTR void toggle_outputs() {
  clock_int_flag = true; //set interrupt flag
}

void setup() {

  Serial.begin(115200,SERIAL_8N1);
  //set up pulse generators for testing
  pinMode(output_pin0,OUTPUT);
  pinMode(output_pin1,OUTPUT);
  
  pcnt_isr_service_install(0);
  // setup hardware pulse counters
  // initialise counter unit 0, channel 0 with signal input GPIO pin and control signal input pin (0 = no control signal input)
  pc0.initialise(input_pin0,PCNT_PIN_NOT_USED);
  pc1.initialise(input_pin1,PCNT_PIN_NOT_USED);
  // count up on negative edges, don't count on positive edges 
  pc0.set_mode(PCNT_COUNT_DIS,PCNT_COUNT_INC,PCNT_MODE_KEEP,PCNT_MODE_KEEP);
  pc1.set_mode(PCNT_COUNT_DIS,PCNT_COUNT_INC,PCNT_MODE_KEEP,PCNT_MODE_KEEP);
  // set glich filter to ignore pulses less than 1000 x 2.5ns
  pc0.set_filter_value(1000);
  pc1.set_filter_value(1000); 
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

  //start clock signal generation
  t0.attach_ms(clock_duration, toggle_outputs);

  Serial.println(F("Start"));
}

void loop() {
  
  if (pc0_int_flag) {
    pc0_int_flag = false; //reset flag
    Serial.print("pc0 : ");
    switch (pc0.event_status()) // test event type
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
    switch (pc1.event_status())
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
  if (clock_int_flag) {
    clock_int_flag = false; //reset flag
    //toggle digial outputs
    if (digitalRead(output_pin0) == HIGH) {
      digitalWrite(output_pin0,LOW);
      digitalWrite(output_pin1,LOW);
    } else {
      digitalWrite(output_pin0,HIGH);
      digitalWrite(output_pin1,HIGH);
    }
  }
}