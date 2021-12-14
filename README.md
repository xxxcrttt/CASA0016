# CASA0016
This is the coursework for CASA0016 -- Making, Designing and Building. 


## Particulate Matter

Particulate matter is defined as particles in air that is less than 10 micrometers in diameter and is able to remain suspended in the air for longer periods of time, especially as higher concentrations in the air represent increasingly serious air pollution.
These particles reach sufficient concentrations and times that they may therefore endanger human health and the living environment.

The diameter of these particles are usually less than 10 micrometers and are difficult for people to distinguish them by naked eyes, but they still have differences in scale. 

## Boards and Sensors

1. Arduino uno WiFi Rev2: [ref](https://store.arduino.cc/products/arduino-uno-wifi-rev2)
<img height="300" src="https://github.com/xxxcrttt/CASA0016/blob/main/Figure/uno%20wifi.jpg"/>

2. PlantTower -- PMS5003: [ref](http://www.plantower.com/en/content/?108.html)
<img height="300" src="https://github.com/xxxcrttt/CASA0016/blob/main/Figure/PMS5003.jpg" />

```C++
SoftwareSerial pmsSerial(8, 9); // Tx->9, Rx->8 
```
|Parameter|Index|unit|
|:-------:|:---:|:--:|
|Range of Measurement|0.3-1.0;1.0-2.5;2.5-10|Micrometer（μ m）|
|Effective Range|0~500|μ g/m³|
|Maximum Range|≥1000|μ g/m³|
|Resolution|1|μ g/m³|
|Standard Volume|0.1|Litre(L)|
|DC Power Supply|4.5-5.5|Volt(V)|
|Standby Current|≤200|Microampere（μ A）|
|Working Temperature Range|-10~+60|℃|
|Storage Temperature Range|-40~+80|℃|
|Physical Size|50×38×21|Millimeter(mm)|

[table](https://www.aqmd.gov/docs/default-source/aq-spec/resources-page/plantower-pms5003-manual_v2-3.pdf) 

3. LCD and LED strip: 

LCD: 

<img height="300" src="https://github.com/xxxcrttt/CASA0016/blob/main/Figure/LCD.jpeg" />

```C++
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
```

LED: 

<img height="200" src="https://github.com/xxxcrttt/CASA0016/blob/main/Figure/LED.jpeg" />

```C++
#define PIN 6
#define NUMPIXELS 8
#define DELAYVAL 500

Adafruit_NeoPixel pixels(NUMPIXELS, PIN);
```

4. Raspberry Pi 4:[ref:](https://www.raspberrypi.com/products/raspberry-pi-4-model-b/)
<img height="300" src="https://github.com/xxxcrttt/CASA0016/blob/main/Figure/Raspberry.jpeg" />



## Process 

1. circuit diagram
<img height="800" src="https://github.com/xxxcrttt/CASA0016/blob/main/Figure/circuit.jpeg"/>



2. actual project
<img height="500" src="https://github.com/xxxcrttt/CASA0016/blob/main/Figure/actual.jpeg"/>



3. Video: 

<video src="https://user-images.githubusercontent.com/56160038/146086203-26830a69-128b-40c8-ae75-d14f6fa6c755.mp4" height="300"> </video>

4. MQTT: 
Publish the data under the topic ```student/CASA0016/ucfnrc0/```, this includes the concentration of 4 different scales of particles. 
<img width="800" src="https://github.com/xxxcrttt/CASA0016/blob/main/Figure/serial1.png"/>
<img width="500" src="https://github.com/xxxcrttt/CASA0016/blob/main/Figure/MQTT.png"/>


5. data storage and Grafana 
<img width="800" src="https://github.com/xxxcrttt/CASA0016/blob/main/Figure/Grafana.png"/>

## Reflection 
1. 3D printing -- more stable 

Due to lack of time I am currently using a paper box, which has the major problem of not being stable enough to be placed well in the environment, so a more stable enclosure can be made by 3D printing. 


2. AQI

Air quality index includes not only Particulate Matter but also other gases such as SO2, O3, CO and so on. 
It is also possible to find other sensors for more versatile air quality monitoring.



## References:
1. https://how2electronics.com/interfacing-pms5003-air-quality-sensor-arduino/
2. https://github.com/jbanaszczyk/pms5003
3. https://www.arduino.cc/en/Tutorial/LibraryExamples/HelloWorld



