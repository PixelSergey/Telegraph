/**    
 *  Telegraph's configuration file. This file is a prototype; it must be
 *  edited before compilation to set up various constants the program uses.
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

#include <ThingerESP8266.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
// WiFiManager library from https://github.com/tzapu/WiFiManager
// Installed from Arduino library manager
#include <WiFiManager.h>

#include <vector>

// General preferences
#define PITCH 600

#define BUTTON D1
#define SWITCH D2
#define SPEAKER D3
#define LED_RED D5
#define LED_GRN D6
#define LED_BLU D7

// Thinger.io preferences
#define USERNAME "Username"
#define DEVICE_ID "Device ID"
#define DEVICE_CREDENTIAL "Device Credential"
