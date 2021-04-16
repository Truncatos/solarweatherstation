#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "XXXXXX";
const char* password = "XXXXXXX";
const char* mqtt_server = "IPXXXXX";

WiFiClient espClient;
PubSubClient client(espClient);
#define INT_STR_SIZE 16
unsigned int raw=0;
float volt=0.0;

Adafruit_BME280 bme;

void setup_wifi() {

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  randomSeed(micros());
}

void reconnect() {
   while (!client.connected()) {
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(A0, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  bool status;
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  if (!client.connected()) {
    reconnect();
  }
  raw = analogRead(A0);
  char buffer[INT_STR_SIZE];
  volt=raw/1023.0;
  volt=volt *4.2;
  dtostrf(volt, 4, 1, buffer);
  client.publish("esp/bme280/battery", String(volt).c_str(), true);
  Serial.println("volts-");
  Serial.print(volt);
  Serial.print(raw);
  float temp = bme.readTemperature();
  dtostrf(temp, 4, 1, buffer);
  client.publish("esp/bme280/temperature", buffer); 
  delay(2000);
  float pressure = bme.readPressure() / 100.0F;
  dtostrf(pressure, 4, 1, buffer);
  client.publish("esp/bme280/pressure", buffer); 
  delay(2000);
  float hum = bme.readHumidity();
  dtostrf(hum, 4, 1, buffer);
  client.publish("esp/bme280/humidity", buffer); 
  delay(2000);  
  ESP.deepSleep(6e8); 
  Serial.println("Tou lixado se isto imprimir");
}

void loop() {
}
