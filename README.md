🔢 NodeMCU 8-bit ALU (Arithmetic Logic Unit)

This project implements an 8-bit ALU (Arithmetic Logic Unit) using a NodeMCU (ESP8266).
It provides a web-based interface to perform arithmetic operations — Addition, Subtraction, Multiplication, and Division — between two 8-bit binary inputs.

The results are displayed both on the web interface and through 9 LEDs connected to the NodeMCU, representing:

1 Carry LED

8 Bit LEDs (Bit7 → Bit0)

🚀 Features

🌐 Wi-Fi Access Point Mode — NodeMCU creates its own network

💻 Interactive Web Interface — Enter binary numbers and choose operations

💡 LED Output Display — Shows result and carry on physical LEDs

⚙️ Example Buttons — Predefined test inputs (ADD, SUB, MUL, DIV)

🔁 Reset Button — Clears all LEDs and inputs

✨ LED Startup Animation

🧠 How It Works

NodeMCU starts as a Wi-Fi Access Point with credentials:

SSID: 8bit_ALU

Password: ALU_12345

Connect your phone/laptop to this Wi-Fi network.

Open any browser and go to http://192.168.4.1.

You’ll see a clean interface:

Enter A and B in binary form (e.g., 00001101)

Select an operation (ADD, SUB, MUL, DIV)

Click Compute

The web page displays:

Result (decimal + binary)

Carry bit

LED indicators representing result bits

LEDs connected to NodeMCU pins D0–D8 display the same output physically.

⚡ Hardware Setup
Function	NodeMCU Pin
Carry LED	D0
Bit 7	D1
Bit 6	D2
Bit 5	D3
Bit 4	D4
Bit 3	D5
Bit 2	D6
Bit 1	D7
Bit 0	D8

Each LED should be connected with a 220Ω resistor to ground.

🧩 Supported Operations
Operation	Description	Carry Handling
ADD	8-bit binary addition	Carry bit shown if overflow (9th bit)
SUB	8-bit subtraction	Carry = 1 if A < B
MUL	8-bit multiplication (lower 8 bits only)	Carry = 0
DIV	8-bit division (A ÷ B)	Carry = 0 (Division by 0 → Result = 0)
💻 Software Setup
1️⃣ Requirements

Arduino IDE

ESP8266 Board Package

Go to: File → Preferences → Additional Boards Manager URLs
Add:

http://arduino.esp8266.com/stable/package_esp8266com_index.json


Then install esp8266 by ESP8266 Community

2️⃣ Libraries Used
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>


(These come with the ESP8266 core — no extra installation required.)

3️⃣ Steps to Upload

Open this code in Arduino IDE.

Select Board: NodeMCU 1.0 (ESP-12E Module).

Connect your NodeMCU via USB.

Click Upload.

Open Serial Monitor at 115200 baud to see setup logs.

🌈 Web Interface Preview

The web page includes:

A dark themed responsive UI

Binary input fields with validation (only 0 and 1, max 8 bits)

Dropdown for operation

Example buttons for quick testing:

ADD → 00001101 + 00000111

SUB → 10000000 - 01000000

MUL → 00010010 × 00000101

DIV → 00101000 ÷ 00000110

Reset button clears all LEDs and input fields

🧾 Code Overview

setup()

Initializes LEDs

Runs LED animation

Starts Wi-Fi AP, DNS, and Web Server

Defines all request handlers

loop()

Handles web requests and DNS operations

displayResult()

Converts binary input → performs operation → updates LEDs and page

makePage()

Dynamically generates the HTML web interface

ledAnimation()

Simple startup LED running effect

⚙️ Example Usage
Input A	Input B	Operation	Result (Decimal)	Result (Binary)	Carry
00001101	00000111	ADD	20	00010100	0
10000000	01000000	SUB	64	01000000	0
00010010	00000101	MUL	74	01001010	0
00101000	00000110	DIV	8	00001000	0
🔧 Troubleshooting
Problem	Solution
Web page not loading	Ensure you’re connected to 8bit_ALU Wi-Fi
LEDs not lighting	Check wiring and pin configuration
Incorrect binary results	Ensure inputs are 8-bit binary strings
Division shows 0	Likely dividing by zero
