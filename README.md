# ESP32 Interactive Desk Watch

**Author:** Puneet Bhati  
**Date:** 2025-09-13

## Overview
This project is an ESP32-based desk watch with a 16x2 LCD display.  
It shows real-time time (12-hour format with seconds) and date/day.  
It includes a web interface to send custom messages wirelessly and toggle the display ON/OFF.

On startup, the LCD scrolls the message:  
**"Made By Puneet Bhati"**  

## Features
- Real-time 12-hour format clock with seconds
- Date and day display
- Web interface for sending messages without page refresh
- Display toggle (ON/OFF) via web
- Scrolling messages for text longer than 16 characters
- Startup scrolling message showing author

## Wiring
- RS: 21
- E: 22
- D4: 19
- D5: 18
- D6: 23
- D7: 25
- VSS: GND
- V0: GND
- RW: GND
- A: 5V
- K: GND

*(Add wiring diagram image if available)*

## License
All rights reserved © 2025 Puneet Bhati
