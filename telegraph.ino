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

#include <vector>
// The preferences file is automatically included here by the Arduino IDE.

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
char recvname[9]; // The name of the receiver
int start_time;
int end_time;
int stop_time;
int pulse_time;
int silence_time;
int last;
int duration;
int command;

std::vector<int> play;
std::vector<int> silence;

void ICACHE_RAM_ATTR timer(){
    int button = digitalRead(BUTTON);
    int delayt = millis() + 30;
    while(millis() < delayt);
    if(button == LOW){
        start_time = millis();
        if(last != LOW) silence_time = start_time - end_time;
    }else{
        end_time = millis();
        if(last != HIGH) pulse_time = end_time - start_time;
    }
    last = button;
}

void setup(){
    Serial.begin(9600);
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(SPEAKER, OUTPUT);
    // Due to the RGB led being common anode, 0 turns an LED fully on and 1023 turns it fully off
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GRN, OUTPUT);
    pinMode(LED_BLU, OUTPUT);

    analogWrite(LED_RED, 1023);
    analogWrite(LED_GRN, 1023);
    analogWrite(LED_BLU, 1023);

    pulse_time = 0;
    silence_time = 0;
    duration = 0;
    command = 0;
    
    attachInterrupt(digitalPinToInterrupt(BUTTON), timer, CHANGE);

    // The most cursed and hacky line of code you will ever see
    // Converts NodeMCU1 <-> NodeMCU0 to find out the recipient's name
    sprintf(recvname, "NodeMCU%d", !((int)DEVICE_ID[7]-48));

    thing.add_wifi(WIFI_SSID, WIFI_PASSWORD);
    
    thing["command"] << [](pson& in){
        int play_t = (int) in["play"];
        int silence_t = (int) in["silence"];
        if(play_t != 0){
            play.push_back(play_t);
        }
        if(silence_t != 0){
            silence.push_back(silence_t);
        }
    };
}

void loop(){
    thing.handle();
    if(pulse_time != 0){
        pson data;
        data["play"] = pulse_time;
        Serial.println(pulse_time);
        pulse_time = 0;
        thing.call_device(recvname, "command", data);
    }
    if(silence_time != 0){
        pson data;
        data["silence"] = silence_time;
        Serial.println(silence_time);
        silence_time = 0;
        thing.call_device(recvname, "command", data);
    }

    if(duration == 0 && (!play.empty() || !silence.empty())){
        if(play.size() >= silence.size()){
            duration = play.front();
            play.erase(play.begin());
            command = 1;
        }else{
            duration = silence.front();
            silence.erase(silence.begin());
            command = 0;
        }
    }
    if(duration != 0){
        stop_time = millis() + duration;
        duration = 0;
        if(command == 1){
            tone(SPEAKER, PITCH);
            analogWrite(LED_GRN, 0);
        }else{
            noTone(SPEAKER);
            analogWrite(LED_GRN, 1023);
        }
    }
    if(millis() > stop_time){
        noTone(SPEAKER);
        analogWrite(LED_GRN, 1023);
        duration = 0;
    }
}
