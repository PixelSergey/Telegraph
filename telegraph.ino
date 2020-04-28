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

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
char recvname[9]; // The name of the receiver

void send_state(int state){
    pson data;
    data["state"] = state;
    thing.call_device(recvname, "state", data);
}

// These two functions call the send_state function separately due to the fact that a function that is
// attached as a callback cannot take any parametres and cannot return anything.
void send_on(){send_state(1);}
void send_off(){send_state(0);}

void setup(){    
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(SPEAKER, OUTPUT);
    // Due to the RGB led being common anode, 0 turns an LED fully on and 1023 turns it fully off
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GRN, OUTPUT);
    pinMode(LED_BLU, OUTPUT);

    attachInterrupt(BUTTON, send_on, FALLING);
    attachInterrupt(BUTTON, send_off, RISING);

    // The most cursed and hacky line of code you will ever see
    // Converts NodeMCU1 <-> NodeMCU0 to find out the recipient's name
    sprintf(recvname, "NodeMCU%d", !((int)DEVICE_ID[7]-48));

    thing.add_wifi(WIFI_SSID, WIFI_PASSWORD);
    
    thing["state"] << [](pson& in){
        int command = (int) in["state"];
        if(command == 1){
            analogWrite(LED_GRN, 0);
            tone(SPEAKER, PITCH);
        }else{
            analogWrite(LED_GRN, 0);
            noTone(SPEAKER);
        }
    };
}

void loop(){
    thing.handle();
}
