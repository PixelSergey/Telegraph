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

/**
 * The preferences file is automatically included here by the Arduino IDE.
 */

int start_time;
int current_time;
int recvstate;
bool stream;
bool recv;
char recvname[9];

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup(){    
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(SPEAKER, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GRN, OUTPUT);
    pinMode(LED_BLU, OUTPUT);
    
    start_time = millis();
    current_time = millis();
    stream = false;
    recv = false;

    // The most cursed and hacky line of code you will ever see
    // Converts NodeMCU1 <-> NodeMCU0 to find out the recipient's name
    sprintf(recvname, "NodeMCU%d", !((int)DEVICE_ID[7]-48));

    thing.add_wifi(WIFI_SSID, WIFI_PASSWORD);

    thing["button"] >> [](pson& out){
        out = digitalRead(BUTTON);
    };
    thing["listener"] << [](pson& in){
        recvstate = in;
    };
    thing["torecv"] = [](){
        recv = !recv;
    };
}

void loop(){
    thing.handle();
    if(recv){
        analogWrite(LED_RED, 0);
        thing.stream();
        if(!recvstate){ // The sender's button is pressed
            tone(SPEAKER, PITCH);
            analogWrite(LED_GRN, 0);
            
        }else{
            noTone(SPEAKER);
            analogWrite(LED_GRN, 1023);
        }
        return;
    }

    analogWrite(LED_RED, 1023);
    int button = digitalRead(BUTTON);
    if(button == LOW){  // The button is pressed; state inverted due to pullup pin
        tone(SPEAKER, PITCH);
        // Due to the RGB led being common anode, 0 turns it fully on and 1023 turns it fully off
        analogWrite(LED_GRN, 0);
        
        if(!stream){
            current_time = millis();
            if(current_time - start_time >= 2500){
                stream = true;
                thing.call_device(recvname, "torecv");
                start_time = millis();
            }
        }else{
            start_time = millis();
        }
    }else{
        noTone(SPEAKER);
        analogWrite(LED_GRN, 1023);

        if(stream){
            current_time = millis();
            if(current_time - start_time >= 5000){
                stream = false;
                thing.call_device(recvname, "torecv");
                start_time = millis();
            }
        }else{
            start_time = millis();
        }
    }
    if(stream){
        thing.stream(thing["button"]);
        analogWrite(LED_BLU, 0);
    }else{
        analogWrite(LED_BLU, 1023);
    }
}
