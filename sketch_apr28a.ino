#include "BluetoothSerial.h"
#include <list>

int PIN_SWITCH = 23;
int PIN_BUTTON = 22;
int buttonState = 0;      // current state of the button
int switchState = 0;
int lastButtonState = 0;  // previous state of the button
int lastSwitchState = 0;  // previous state of the switch
unsigned long lastDebounceTime = 0;  // the last time the button was toggled
unsigned long debounceDelay = 50;    // debounce time in milliseconds
unsigned long startTime = 0;  // the time the button was last pushed
unsigned long endTime = 0;    // the time the button was pushed again
std::list<int> list;
bool n = true;

BluetoothSerial SerialBT;

void setup() {
  SerialBT.begin("ESP32test"); // set the device name to "ESP32test"
  Serial.begin(9600);
  pinMode(PIN_SWITCH, INPUT);
  pinMode(PIN_BUTTON, INPUT);
}

void loop() {
  
  int state = digitalRead(PIN_SWITCH);
  
  //if (state != switchState) {

    switchState = state;
    
    if (switchState == HIGH) {
      count();
    } else {
      //if(n) {
        //Serial.println("all cond...");
        send_list();
      //}
      n = false;
    }
      
  //}
  lastSwitchState = state;
}

void send_list() {
  Serial.println("Waiting request...");
  delay(1000);
//  for (int n : list){
//    Serial.println(String(n));
//  }
//  int size = list.size();
//      Serial.print("Size: ");
//      Serial.println(size);
      
  if (SerialBT.available()) { // check if there is incoming data over Bluetooth
    Serial.println("Available!");
    String request = SerialBT.readString(); // read the request from the Android phone
    Serial.print("Request: ");
    Serial.println(request);
    if (request == "get_data") { // if the request is "get_data", send the array
      Serial.println("Configuring...");
      
      int size = list.size();
      Serial.print("Size: ");
      Serial.println(size);
      String str = "";

      for (int n : list){
        String m = String(n);
        str += m;
        str += ",";
      }
      

      
     
      Serial.println("String: ");
      Serial.println(str);
      SerialBT.println(str); // send the string over Bluetooth
    }
  }
}

void count() {
    int reading = digitalRead(PIN_BUTTON);

    // check if the button state has changed
    if (reading != lastButtonState) {
      lastDebounceTime = millis();
    }

    // debounce the button
    if (millis() - lastDebounceTime > debounceDelay) {
      // if the button state has changed, update the button state
      if (reading != buttonState) {
        buttonState = reading;

        // if the button is pressed down, record the start time
        if (buttonState == LOW) {
          startTime = millis();
        }
        // if the button is released, record the end time and calculate the duration
        else {
          endTime = millis();
          int duration = (endTime - startTime) / 1000;
          Serial.print("Duration: ");
          Serial.print(duration);
          Serial.println(" seconds");
          list.push_back(duration);
          for (int n : list){
            Serial.println(String(n));
          }
        }
      }
    }

    // update the last button state
    lastButtonState = reading;
  }
