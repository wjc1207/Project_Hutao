# Project_Hutao

Project_Hutao is a IoT project. Its main part is a desktop ornament made from arduino nano with a light sensor, a DHT22 sensor, a voice recognetion v3 module, an OLED screen, a RGB light, a rocker and most important, a hutao shake toy. The technique includes the arduino, FPGA, 3D printing, SoildWorks, Winform, web and so on. The basic idea is from this [link](https://www.hackster.io/yuwork58426/voice-control-toy-and-display-using-xiao-and-edge-impulse-592bb4), but with different methods.

---

### **the hardware**
#### the necessary part:
1. Hutao Shake Toy
  - **more details see from this [Link](https://space.bilibili.com/8549577?spm_id_from=333.337.0.0). Origin Author: Bilibili@机智的Kason.**
  - 3D printing parts
  - some toothpicks
  - a paper with some color printed pattern
  - direct-current motor
  - 502 glue
  - a knife
  - two wires
2. an Arduino nano/uno R3
3. a voice recognizition v3.0 board
4. 0.96' 128x64 1306 OLED screen
5. a BJT with over 400mA load current

the total cost is about **150 CNY** in Taobao in China.

#### the aditional part:
1. DHT22 module
2. light sensor with a 10k resistor
3. a RGB light with three 100 Ohms resistors
4. a breadboard
5. a rocker
6. a 3D printer (useful for geeks)

the total cost is about **18 CNY** in Taobao in China. (of course exclude the 3D printer)

#### the useless part:
1. FPGA (asked by my teacher)

---

### **the software**
#### the necessary part:
1. Arduino IDE

#### the aditional part:
1. Cura/PursaSlicer
2. SoildWorks/fushion360/Blender

#### the useless part: (asked by my teacher)
1. Quartus 2 (for FPGA part)
2. Visio Studio (for winForm)
3. Dreamweaver (for web design)

---

### **PINs definition**

Pseudocode

`#define LIGHTSENSOR (A0)`

`#define X (A1)

`#define Y (A2)`

`#define SWITCH (A3)`

`#define SDA (A4)`

`#define SCL (A5)`

`#define VR_TX 2`

`#define VR_RX 3`

`#define MOTOR 6`

`#define DHTPIN 12`

`#define BLUEPIN 8`

`#define GREENPIN 9`

`#define REDPIN 10`
