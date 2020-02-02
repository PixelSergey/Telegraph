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

void setup(){
    pinMode(BUTTON, INPUT_PULLUP);
    pinMode(SPEAKER, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop(){
    int state = digitalRead(BUTTON);
    if(state == LOW){ // The button is pressed; state inverted due to pullup pin
        tone(SPEAKER, PITCH);
    }else{
        noTone(SPEAKER);
    }
    digitalWrite(LED_BUILTIN, state);
}
