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

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0);

char recvname[9]; // The name of the receiver
unsigned long init_time;
int current_millis;

int last;
int out_command;

unsigned long in_init_time;
std::vector<int> in_millis;
std::vector<int> in_command;

void ICACHE_RAM_ATTR timer(){
    int button = digitalRead(BUTTON);
    if(last != button){
        out_command = button;
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

    last = 0;
    out_command = -1;
    
    attachInterrupt(digitalPinToInterrupt(BUTTON), timer, CHANGE);
    
    // The most cursed and hacky line of code you will ever see
    // Converts NodeMCU1 <-> NodeMCU0 to find out the recipient's name
    sprintf(recvname, "NodeMCU%d", !((int)DEVICE_ID[7]-48));
    
    thing.add_wifi(WIFI_SSID, WIFI_PASSWORD);
    
    thing.handle();
    timeClient.begin();
    timeClient.update();
    current_millis = millis();
    init_time = (unsigned long)(timeClient.getEpochTime() - current_millis/1000);
    Serial.println();
    Serial.printf("Init. time: %d\n", init_time);
    Serial.printf("Millis. time: %d\n", current_millis);
    timeClient.end();
    
    thing["command"] << [](pson& in){
        in_init_time = (unsigned long) in["inittime"];
        Serial.printf("Init Time: %d\n", in_init_time);
        in_command.push_back((int) in["command"]);
        Serial.printf("Command: %d\n", in_command.front());
        in_millis.push_back((int) in["millis"]);
        Serial.printf("Millis: %d\n", in_millis.front());
    };
}

void loop(){
    thing.handle();
    current_millis = millis();
    
    if(out_command != -1){
        pson data;
        data["inittime"] = init_time;
        data["command"] = out_command;
        data["millis"] = current_millis;
        Serial.println(out_command);
        out_command = -1;
        thing.call_device(recvname, "command", data);
    }
    if(!in_command.empty() && !in_millis.empty()){
        // Check the whole number of seconds first
        unsigned long start_sec = in_init_time + (unsigned long)(in_millis.front()/1000);
        unsigned long now_sec = init_time + (unsigned long)(current_millis/1000);
        if(start_sec >= now_sec){
            // And then check the millisecond part
            int start_ms = in_millis.front() - 1000*(int)(in_millis.front()/1000);
            int now_ms = current_millis - 1000*(int)(current_millis/1000);
            if(start_ms >= now_ms){
                if(in_command.front() == LOW){
                    Serial.println("Activated");
                    analogWrite(LED_GRN, 0);
                    tone(SPEAKER, PITCH);
                }else{
                    Serial.println("De-activated");
                    analogWrite(LED_GRN, 1023);
                    noTone(SPEAKER);
                }
                in_command.erase(in_command.begin());
                in_millis.erase(in_millis.begin());
            }
        }
    }
}
