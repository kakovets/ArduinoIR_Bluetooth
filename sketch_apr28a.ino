#include <list>

#include "BluetoothSerial.h"

int PIN_SWITCH = 23;
int PIN_IR = 22;
int PIN_LED = 2;
int IRstate = 1;  // current state of the button
int switchState = 0;
int lastIRState = 0;          // previous state of the button
int lastSwitchState = 0;      // previous state of the switch
unsigned long startTime = 0;  // the time the button was last pushed
unsigned long endTime = 0;    // the time the button was pushed again
std::list<double> list;
bool first = false;

BluetoothSerial SerialBT;

void setup() {
    SerialBT.begin("ESP32test");  // set the device name to "ESP32test"
    Serial.begin(9600);
    pinMode(PIN_SWITCH, INPUT);
    pinMode(PIN_IR, INPUT);
    pinMode(PIN_LED, OUTPUT);
}

void loop() {
    int state = digitalRead(PIN_SWITCH);

    switchState = state;

    if (switchState == HIGH) {
        count();
    } else {
        send_list();
    }

    lastSwitchState = state;
}

void count() {
    int reading = digitalRead(PIN_IR);

    if (reading != IRstate) {
        IRstate = reading;

        // if the button is pressed down, record the start time
        if (IRstate == HIGH) {
            startTime = millis();
            digitalWrite(PIN_LED, LOW);
        }
        // if the button is released, record the end time and calculate the
        // duration
        else {
            digitalWrite(PIN_LED, HIGH);
            endTime = millis();
            double duration = (endTime - startTime) / 1000.0;
            Serial.print("Duration: ");
            Serial.print(duration);
            Serial.println(" seconds");
            if (first) {
                list.push_back(duration);
            }
            first = true;

            Serial.print("List: ");
            for (double n : list) {
                Serial.print(n, 3);
                Serial.print(" ");
            }
            Serial.println();
        }
    }

    // update the last button state
    lastIRState = reading;
}

void send_list() {
    // Serial.println("Waiting request...");
    // delay(1000);

    if (SerialBT.available()) {  // check if there is incoming data over Bluetooth
        Serial.println("Available!");
        String request = SerialBT.readString();  // read the request from the Android phone
        Serial.print("Request: ");
        Serial.println(request);
        if (request == "get_data") {  // if the request is "get_data", send the array
            Serial.println("Configuring...");

            int size = list.size();
            Serial.print("Size: ");
            Serial.println(size);
            String str = "";

            for (double n : list) {
                String m = String(n, 3);
                str += m;
                str += ",";
            }
            Serial.println("String: ");
            Serial.println(str);
            SerialBT.println(str);  // send the string over Bluetooth
        }
    }
}
