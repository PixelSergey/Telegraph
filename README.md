# Telegraph

[![Listen to this README here](meta/listen.png "Listen to this README here")](README.wav)

A wireless [morse] telegraph system for the NodeMCU ESP8266 chip

## General info

Have you ever wanted to be a 1920s radio operator but didn't want to get a wireless telegraphy setup,
a retro morse key and an amateur radio license?

Fear not, Telegraph will replace all three with a NodeMCU and some hardware!

- Uses the NodeMCU v1.0, ESP8266-12E v2 Amica board.
  - There are multiple versions of the NodeMCU with confusing names. This board is the newest official one and has a
standard breadboard size. See [this article](https://frightanic.com/iot/comparison-of-esp8266-nodemcu-development-boards/) for reference.
- Uses the [Thinger.io](https://thinger.io/) service to communicate between devices
- Currently only supports communication between two devices, which is the maximum you can have with a free Thinger.io plan
- The morse key for this project uses [this 3D-printed design](https://www.thingiverse.com/thing:3167343)
- Note: pulses less than 7ms in length will be ignored as debounce noise

## Installation

1. Build the circuit as shown in the diagrams below
1. Install the [Arduino IDE](https://www.arduino.cc/en/main/software)
1. Install the [NodeMCU Add-On](https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/)
1. Install the [Thinger.io library](https://docs.thinger.io/devices/arduino) from the Arduino Library Manager
1. Install the [WiFiManager library](https://github.com/tzapu/WiFiManager) by TZAPU from the Arduino Library Manager
1. Set up two Thinger.io devices, called NodeMCU0 and NodeMCU1
1. Modify the preferences under the `preferences.ino` file to include your Thinger credentials
1. Connect the NodeMCUs to your computer via USB and upload the code

## Usage

- The first time you start the Telegraph, it will go into setup mode and shine a red light
  - This will make it into a WiFi beacon
  - Connect to the network named `Telegraph` with your phone or other device
  - Fill in your WiFi SSID and password into the login webpage
  - After you finish, Telegraph should connect to the provided network and all lights should turn off
  - If the network credentials were incorrect or the device fails to connect, it will simply re-enter setup mode
- Perform the setup procedure and turn on both Telegraphs
- Press the button on one and it will send the signal in real time to the other one
- Outgoing signals flash the green LED, and incoming signals flash the blue LED on your device
  - Both signals beep the speaker at the same pitch

## Circuit

The circuit contains a morse key (which can be any button), a speaker, and an RGB LED light.
These are hooked up using resistors and capacitors. A breadboard also helps a lot. The circuit is as follows:

![Breadboard diagram](meta/Telegraph_bb.png) ![Circuit diagram](meta/Telegraph_schem.png)

## Credits

- Ega on the Thinger.io forum for [this wild goose chase of a conversation](https://community.thinger.io/t/how-to-receive-a-stream-on-a-nodemcu/3113)
- Samuel Morse
- [How to exit vim](https://stackoverflow.com/questions/11828270/how-do-i-exit-the-vim-editor)

