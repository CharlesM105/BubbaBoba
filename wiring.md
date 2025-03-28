{\rtf1\ansi\ansicpg1252\cocoartf2821
\cocoatextscaling0\cocoaplatform0{\fonttbl\f0\fswiss\fcharset0 Helvetica;}
{\colortbl;\red255\green255\blue255;}
{\*\expandedcolortbl;;}
\margl1440\margr1440\vieww11520\viewh8400\viewkind0
\pard\tx720\tx1440\tx2160\tx2880\tx3600\tx4320\tx5040\tx5760\tx6480\tx7200\tx7920\tx8640\pardirnatural\partightenfactor0

\f0\fs24 \cf0 ---\
\
## \uc0\u9989  `wiring.md`\
\
```markdown\
# \uc0\u55357 \u56588  Wiring Guide \'96 Yummy Boba Machine\
\
This guide explains how to wire each component of the boba machine to the Arduino Mega.\
\
---\
\
## \uc0\u55357 \u56543  LCD (I2C 20x4 Display)\
- VCC \uc0\u8594  5V\
- GND \uc0\u8594  GND\
- SDA \uc0\u8594  SDA (Pin 20 on Mega)\
- SCL \uc0\u8594  SCL (Pin 21 on Mega)\
\
---\
\
## \uc0\u55357 \u56577  Rotary Encoder\
- GND \uc0\u8594  GND\
- VCC \uc0\u8594  5V\
- CLK \uc0\u8594  Pin 2\
- DT \uc0\u8594  Pin 3\
- SW (Button press) \uc0\u8594  Pin 4\
\
---\
\
## \uc0\u55358 \u56800  RFID Reader (MFRC522)\
- SDA \uc0\u8594  Pin 53\
- SCK \uc0\u8594  Pin 52\
- MOSI \uc0\u8594  Pin 51\
- MISO \uc0\u8594  Pin 50\
- IRQ \uc0\u8594  Not Connected\
- GND \uc0\u8594  GND\
- RST \uc0\u8594  Pin 49\
- 3.3V \uc0\u8594  3.3V\
\
---\
\
## \uc0\u55356 \u57088  Stepper Motors (via Driver)\
- STEP \uc0\u8594  Pins 8 / 9 / 10\
- DIR \uc0\u8594  Pins 5 / 6 / 7\
- EN (optional) \uc0\u8594  GND\
- VCC \uc0\u8594  12V external power\
- GND \uc0\u8594  Shared ground with Arduino\
\
---\
\
## \uc0\u55356 \u57262  Limit Switches\
- COM \uc0\u8594  GND\
- NO \uc0\u8594  Digital Pins 22, 23, 24 (or any)\
\
---\
\
## \uc0\u55356 \u57243 \u65039  Servo Motor\
- Signal \uc0\u8594  Pin 11\
- VCC \uc0\u8594  5V\
- GND \uc0\u8594  GND\
\
---\
\
## \uc0\u55356 \u57096  NeoPixel LED Strip\
- DIN \uc0\u8594  Pin 12\
- VCC \uc0\u8594  5V (External source recommended)\
- GND \uc0\u8594  Common ground\
\
---\
\
## \uc0\u9889  Power Supply & Buck Converter\
- 12V \uc0\u8594  Main power input\
- 5V Output \uc0\u8594  Logic components (e.g. servo, RFID)\
- GND connected across all systems\
\
---\
\
## \uc0\u55357 \u56488  Fans\
- +12V \uc0\u8594  Power rail\
- GND \uc0\u8594  Ground\
\
---\
\
## \uc0\u55357 \u56577  Relay Modules\
- IN1 \uc0\u8594  Pin 40 (example)\
- VCC \uc0\u8594  5V\
- GND \uc0\u8594  GND\
\
---\
\
## \uc0\u55357 \u56567  Wiring Diagram\
\
> Diagram will be added once the final wiring is complete.\
\
<!-- ![Wiring Diagram](docs/wiring_diagram.png) -->}