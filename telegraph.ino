/**    
 *  Telegraph's main program file.
 *  Copyright (C) 2020 PixelSergey
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

// The preferences file is automatically included here by the Arduino IDE.

// System variables
ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
char recvname[9]; // The name of the receiver
long dt; // The time difference between both NodeMCUs

// Output variables
int out_command;
int last;

// Input variables
std::vector<unsigned long> in_millis;
std::vector<int> in_command;

/**
 * Interrupt function. Immediately interrupts the main program and
 * jumps here when a button state change is detected.
 * The ICACHE_RAM_ATTR flag is needed to put the function in RAM,
 * as that is required to avoid a crash.
 */
void ICACHE_RAM_ATTR on_interrupt(){
    int button = digitalRead(BUTTON);
    if(last != button) out_command = button;
    last = button;
}

void setup(){
    Serial.begin(9600);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(SPEAKER, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GRN, OUTPUT);
    pinMode(LED_BLU, OUTPUT);
    // Due to the RGB led having a common anode, analog 0 turns
    // an LED fully on and analog 1023 turns it fully off
    analogWrite(LED_RED, 1023);
    analogWrite(LED_GRN, 1023);
    analogWrite(LED_BLU, 1023);
    
    last = 0;
    out_command = -1;

    // Attach the interrupt function to any change in state of the button
    attachInterrupt(digitalPinToInterrupt(BUTTON), on_interrupt, CHANGE);
    
    // The most cursed and hacky line of code you will ever see
    // Converts NodeMCU1 <-> NodeMCU0 to find out the recipient's name
    sprintf(recvname, "NodeMCU%d", !((int)DEVICE_ID[7]-48));
    
    thing.add_wifi(WIFI_SSID, WIFI_PASSWORD);
    
    thing["command"] << [](pson& in){
        in_command.push_back((int) in["command"]);
        //Serial.printf("Command: %d\n", in_command.front());
        in_millis.push_back((unsigned long) in["millis"]);
        //Serial.printf("Millis: %d\n", in_millis.front());
    };
    thing["timesync"] << [](pson& in){
        dt = millis() - (unsigned long) in["millis"];
        Serial.printf("Calculated dt = %d", dt);
        pson data;
        data["dt"] = -dt;
        thing.call_device(recvname, "timeresponse", data);
    };
    thing["timeresponse"] << [](pson& in){
        dt = (long) in["dt"];
        Serial.printf("Received dt = %d", dt);
    };

    // Attempt to call the other NodeMCU right off the bat.
    // The first one to boot will fail, the second will succeed.
    thing.handle();
    pson data;
    data["millis"] = millis();
    thing.call_device(recvname, "timesync", data);
}

void loop(){
    thing.handle();
    
    if(out_command != -1){
        if(out_command == LOW){
            analogWrite(LED_GRN, 0);
            tone(SPEAKER, PITCH);
        }else{
            analogWrite(LED_GRN, 1023);
            noTone(SPEAKER);
        }
        
        pson payload;
        payload["command"] = out_command;
        payload["millis"] = millis();
        Serial.println(out_command);
        out_command = -1;
        thing.call_device(recvname, "command", payload);
    }
    if(!in_command.empty() && !in_millis.empty()){
        if(millis() > in_millis.front() + dt + 1000){
            // The button emits a LOW voltage when pressed due to the INPUT_PULLUP mode
            if(in_command.front() == LOW){
                analogWrite(LED_GRN, 0);
                tone(SPEAKER, PITCH);
                Serial.println("Activated");
            }else{
                analogWrite(LED_GRN, 1023);
                noTone(SPEAKER);
                Serial.println("De-activated");
            }
            in_command.erase(in_command.begin());
            in_millis.erase(in_millis.begin());
        }
    }
}
