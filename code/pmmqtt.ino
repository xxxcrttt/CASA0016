// Project for CASA0016 -- PM detector
// Board: Arduino Uno WiFi Rev2
// Sensor: PMS5003 (Plantower)

// install these from libraries
#include <SoftwareSerial.h>
#include <Wire.h>
#include "PMS.h"
#include <ezTime.h> // time
#include <WiFiNINA.h> // wifi
#include <LiquidCrystal.h> // LCD control
#include <PubSubClient.h> // MQTT messages
#include <Adafruit_NeoPixel.h> // neopixel

// sensor define:
// Vcc to arduino 5V
// GND to arduino GND
// Rx -> pin 8, Tx -> pin 9
SoftwareSerial PMSSerial(8, 9);

// LCD define:
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//neopixel define:
#define PIN 6
#define NUMPIXELS 8
#define DELAYVAL 500

Adafruit_NeoPixel pixels(NUMPIXELS, PIN);

// Wifi and MQTT 
#include "arduino_secrets.h"
//char ssid[] = SECRET_SSID;  // network ssid 
//char pass[] = SECRET_PASS;  // network password
//char mqttuser[] = SECRET_MQTTUSER; // mqtt username 
//char mqttpass[] = SECRET_MQTTPASS; // mqtt password

int status = WL_IDLE_STATUS;

const char* mqtt_server = "mqtt.cetools.org";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// Date and Time 
Timezone GB;


PMS pms(PMSSerial);
PMS::DATA data;
int pms1_0, pms2_5, pms10_0;

void setup() {
  Serial.begin(9600);
  PMSSerial.begin(9600);

  // run wifi initialisation function
  startWifi();
  //startDate();

  // start MQTT server
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);


  //LCD initialised:
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  lcd.print("PM sensor");
  lcd.setCursor(0,1);
  lcd.print("By Cherry!");
  delay(1000);

  //neopixel
  pixels.begin();
}

void loop() {

  // pms data reading 
  while(!pms.read(data)) {} // whether sensor is working
  pms1_0 = data.PM_AE_UG_1_0;
  pms2_5 = data.PM_AE_UG_2_5;
  pms10_0 = data.PM_AE_UG_10_0;
  Serial.print("PM 1.0 (ug/m3): ");
  Serial.println(pms1_0);
  Serial.print("PM 2.5 (ug/m3): ");
  Serial.println(pms2_5);
  Serial.print("PM 10.0 (ug/m3): ");
  Serial.println(pms10_0);
  Serial.println();
  delay(500);

  //lcd visualisation
  lcd_update();

  //neopixel
  neopixel_update();

  // send MQTT
  sendMQTT();
  client.loop();
}


void lcd_update() {
  lcd.clear();
  //first line
  lcd.setCursor(0,0);
  //need to print 
  lcd.print("1.0um:");
  lcd.print(pms1_0);
  lcd.print(" ");
  lcd.print("2.5um:");
  lcd.print(pms2_5);
  lcd.print(" ");
  //second line
  lcd.setCursor(0,1);
  lcd.print("10.0um:");
  lcd.print(pms10_0);

  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    // score one position left: 
    lcd.scrollDisplayLeft();
    delay(500);
  }
  //delay(500);
}

void neopixel_update(){
  pixels.clear();
  if (pms2_5 >= 23 && pms2_5 <= 35) {
    pixels.setPixelColor(0, 190, 190, 190);
    pixels.show();
    delay(DELAYVAL);
  }
  else if (pms2_5 >= 36 && pms2_5 <= 41) {
    for (int i=0; i<1;i++) {
      pixels.setPixelColor(i, 190, 190, 190);
      pixels.show();
      delay(DELAYVAL);
    }
  }
  else if (pms2_5 >= 42 && pms2_5 <= 53) {
    for (int i=0; i<2;i++) {
      pixels.setPixelColor(i, 190, 190, 190);
      pixels.show();
      delay(DELAYVAL);
    }
  }
  else if (pms2_5 >= 54 && pms2_5 <= 64) {
    for (int i=0; i<3;i++) {
      pixels.setPixelColor(i, 190, 190, 190);
      pixels.show();
      delay(DELAYVAL);
    }
  }
  else if (pms2_5 >= 65 && pms2_5 <= 70) {
    for (int i=0; i<4;i++) {
      pixels.setPixelColor(i, 190, 190, 190);
      pixels.show();
      delay(DELAYVAL);
    }
  }
  else if (pms2_5 >= 71) {
    for (int i=0; i<5;i++) {
      pixels.setPixelColor(i, 190, 190, 190);
      pixels.show();
      delay(DELAYVAL);
    }
  }
}

// below are the wifi settings 
void startWifi() {
  // Start by connecting to a wifi network
  Serial.println();
  Serial.print("connecting to");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  //check to see if connected and wait 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("...");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void syncDate() {
  // get real date and time
  waitForSync();
  Serial.println("UTC:" + UTC.dateTime());
  GB.setLocation("Europe/London");
  Serial.println("London time:" + GB.dateTime());
}


void sendMQTT() {
  
  // try to reconnected if not
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // pm1.0
  snprintf(msg, 50, "%.0i", pms1_0);
  Serial.println(msg);
  client.publish("student/CASA0016/ucfnrc0/pm1.0: ", msg);

  // pm2.5
  snprintf(msg, 50, "%.0i", pms2_5);
  Serial.println(msg);
  client.publish("student/CASA0016/ucfnrc0/pm2.5: ", msg);

  // pm10.0
  snprintf(msg, 50, "%.0i", pms10_0);
  Serial.println(msg);
  client.publish("student/CASA0016/ucfnrc0/pm10.0: ", msg);
  
  
}


void reconnect() {
  //Loop until reconnect
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    //create a random client TD
    String clientId ="ESP8266Client --";
    clientId += String(random(0xffff), HEX);

    //Attempt to connect
    if (client.connect(clientId.c_str(), mqttuser, mqttpass)) {
      Serial.println("connected");

      // and subscribe to messages on broker
      client.subscribe("student/CASA0016/inTopic");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 sec");

      //wait 5 sec before retrying
      delay(500);
    }
  }
}


//Execute code when a msg is received from the MQTT 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
