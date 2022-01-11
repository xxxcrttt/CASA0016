// Project for CASA0016 -- Test particles in the air 
// Using: PMS5003 sensor 
// LCD to show the data.

// install these from libraries 
#include <SoftwareSerial.h> 
#include <Wire.h>
#include <ezTime.h> // time 
#include <WiFiNINA.h> // wifi 
#include <LiquidCrystal.h> // LCD control 
#include <PubSubClient.h> // MQTT messages
#include <Adafruit_NeoPixel.h> // neopixel


// Sensor define 
SoftwareSerial pmsSerial(8, 9); // Tx->9, Rx->8 


// LCD define 
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// neopixel define
#define PIN 6
#define NUMPIXELS 8
#define DELAYVAL 500

Adafruit_NeoPixel pixels(NUMPIXELS, PIN);


// WiFi and MQTT 
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


void setup() {
  // need to set up LED and BUILIN_LED smt. 
  // like pinMode, 

  //MQTT 
  Serial.begin(9600);
  delay(100); 
  // pinMode()

  // run wifi initialisation functions
  startWifi();
  syncDate();

  // start MQTT server 
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);

  //pms 
  pmsSerial.begin(9600);

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

//show pms data 
struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;  
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_100um;
  uint16_t unused;
  uint16_t checksum;  
};

struct pms5003data data;

void lcd_update() {
  lcd.clear();
  //first line
  lcd.setCursor(0,0);
  // need to print 
  lcd.print(" 0.3um: ");
  lcd.print(data.particles_03um);
  lcd.print(" ");
  lcd.print(" 0.5um: ");
  lcd.print(data.particles_05um);
  
  // second line 
  lcd.setCursor(0,1);
  lcd.print(" 1.0um: ");
  lcd.print(data.particles_10um); 
  lcd.print(" ");
  lcd.print(" 2.5um: ");
  lcd.print(data.particles_25um);   
  // score 16 positions to the left 
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    // score one position left: 
    lcd.scrollDisplayLeft();
    delay(500);
  }
  //delay(500);
  
}


void loop() {

  // pms data reading 
  if (readPMSdata(&pmsSerial)) {
    // reading data was successful!
    Serial.println();
    Serial.println("---------------------------------------");
    Serial.print("Particles > 0.3um / 0.1L air:"); 
    Serial.println(data.particles_03um);
    Serial.print("Particles > 0.5um / 0.1L air:"); 
    Serial.println(data.particles_05um);
    Serial.print("Particles > 1.0um / 0.1L air:"); 
    Serial.println(data.particles_10um);
    Serial.print("Particles > 2.5um / 0.1L air:"); 
    Serial.println(data.particles_25um);
    Serial.print("Particles > 10.0 um / 0.1L air:"); 
    Serial.println(data.particles_100um);
    Serial.println("---------------------------------------");
  }
  delay(500);

  
  //lcd visualisation
  lcd_update();

  //neopixels 
  pixels.clear();
  if( data.particles_03um >= 400 && data.particles_03um < 600 ) {
    pixels.setPixelColor(0, 190, 190, 190);
    pixels.show();
    delay(DELAYVAL);
  }
  else if(data.particles_03um >= 600 && data.particles_03um < 800 ) {
    pixels.setPixelColor(2, 190, 190, 190);
    pixels.show();
    delay(DELAYVAL);
  }
  else if(data.particles_03um >= 800 && data.particles_03um < 1000 ) {
    pixels.setPixelColor(4, 190, 190, 190);
    pixels.show();
    delay(DELAYVAL);
  }
  else if(data.particles_03um >= 1000 && data.particles_03um < 1200 ) {
    pixels.setPixelColor(6, 190, 190, 190);
    pixels.show();
    delay(DELAYVAL);
  }
  

  // send MQTT 
  sendMQTT();
  client.loop();
}


// funciton void  -- for PM2.5 
boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }

  // Read a byte at a time until '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }

  // Read all 32 bytes
  if (s->available() < 32) {
    return false;
  }

  uint8_t buffer[32];
  uint16_t sum = 0;
  s->readBytes(buffer, 32);

  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }

  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);
  
  return true;
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

  // particles > 0.3um: 
  snprintf (msg, 50, "%.0i", data.particles_03um);
  Serial.print("Publish message for  ");
  Serial.println(msg);
  client.publish("student/CASA0016/ucfnrc0/particles03um", msg);

  // particles > 0.5um: 
  snprintf (msg, 50, "%.0i", data.particles_05um);
  Serial.print("Publish message for  ");
  Serial.println(msg);
  client.publish("student/CASA0016/ucfnrc0/particles05um", msg);

  // particles > 1.0 um:
  snprintf (msg, 50, "%.0i", data.particles_10um);
  Serial.print("Publish message for  ");
  Serial.println(msg);
  client.publish("student/CASA0016/ucfnrc0/particles10um", msg);

  // particles > 2.5 um:
  snprintf (msg, 50, "%.0i", data.particles_25um);
  Serial.print("Publish message for  ");
  Serial.println(msg);
  client.publish("student/CASA0016/ucfnrc0/particles25um", msg);
  
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
