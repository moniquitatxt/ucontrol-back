#include <Wire.h>
#include "DHT.h"
#include <PubSubClient.h>
#if defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#endif
#include <SoftwareSerial.h>

#define DHTTYPE DHT21  // DHT 21 (AM2301)
uint8_t DHTPin = 14;   // D5 pin on nodeMCU board, GPIO pin 14 - connected DATA from AM2301_sensor

DHT dht(DHTPin, DHTTYPE);

// Wifi
const char* ssid = "luisa";
const char* password = "123luisa";

// MQTT Server
const char* mqtt_server = "172.24.62.241";
const char* mqtt_username = "ucontrol";
const char* mqtt_password = "Ucontrol123";
const int mqtt_port = 1884;

// Topicos
const String TH_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Temperatura y humedad del laboratorio";
const String TOPIC_TEMPERATURE = TH_TOPIC + " / Temperatura";
const String TOPIC_HUMIDITY = TH_TOPIC + " / Humedad";
const String WATER_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Sensor de agua";
const String MOV_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Sensor de movimiento";
const String VIB_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Sensor de vibraciones";
const String SOIL_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Sensor de tierra para las macetas";

float hum;   //Stores humidity value
float temp;  //Stores temperature value

// Secure WiFi Connectivity Initialisation
WiFiClient espClient;

// MQTT Client Initialisation Using WiFi Connection
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

unsigned long lastHumiDataSendTime = 0;
unsigned long lastDHTDataSendTime = 0;
int tiempo = 10000;

SoftwareSerial NodeMCU_SS(D1, D2);

// Time zone info
#define TZ_INFO "UTC+4"

char c;
String dataIn;

// Humedad del suelo
const int AirValue = 820;
const int WaterValue = 360;
int intervals = (AirValue - WaterValue) / 3;
const int SoilSensorPin = A0;
int soilMoistureValue = 0;
int soilmoisturepercent = 0;

/************* Connect to WiFi ***********/
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(ssid, password);
  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/************* Connect to MQTT Broker ***********/
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(TH_TOPIC.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(TOPIC_TEMPERATURE.c_str());
      client.subscribe(TOPIC_HUMIDITY.c_str());
      client.subscribe(WATER_TOPIC.c_str());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);

  delay(500);
  NodeMCU_SS.begin(115200);
  dht.begin();
}

void loop() {
  while (NodeMCU_SS.available() > 0) {
    c = NodeMCU_SS.read();
    if (c == '\n') {
      break;
    } else {
      dataIn += c;
    }
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (c == '\n') {
    Serial.println(dataIn);

    unsigned long now = millis();
    if (now - lastMsg > 2000) {
      lastMsg = now;

      // Humedad del suelo (no viene de la arduino)
      if (strcmp(dataIn.c_str(), "Soil") == 0 && now - lastHumiDataSendTime >= tiempo) {
        lastHumiDataSendTime = now;
        soilMoistureValue = analogRead(SoilSensorPin);
        Serial.println(soilMoistureValue);
        soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
        Serial.print("Soil percent: ");
        Serial.println(soilmoisturepercent);
        client.publish(SOIL_TOPIC.c_str(), String(soilmoisturepercent).c_str());
      }

      // Temperatura y humedad (no vienen de la arduino)
      if (now - lastDHTDataSendTime >= tiempo) {
        lastDHTDataSendTime = now;
        hum = dht.readHumidity();
        temp = dht.readTemperature();
        Serial.println(hum);
        Serial.println(temp);
        client.publish(TOPIC_TEMPERATURE.c_str(), String(temp).c_str());
        client.publish(TOPIC_HUMIDITY.c_str(), String(hum).c_str());
      }

      // Movimiento (viene de la Arduino)
      if (strcmp(dataIn.c_str(), "Movimiento 1") == 0) {
        Serial.println("Motion detected!");
        client.publish(MOV_TOPIC.c_str(), String(1).c_str());
      } else if (strcmp(dataIn.c_str(), "Movimiento 0") == 0) {
        Serial.println("NO Motion detected!");
        client.publish(MOV_TOPIC.c_str(), String(0).c_str());
      }

      // Vibracion (viene de la arduino)
      if (strcmp(dataIn.c_str(), "Vibracion 1") == 0) {
        Serial.println("Vibracion detectada");
        client.publish(VIB_TOPIC.c_str(), String(1).c_str());
      } else if (strcmp(dataIn.c_str(), "Vibracion 0") == 0) {
        client.publish(VIB_TOPIC.c_str(), String(0).c_str());
      }

      // Agua (viene de la arduino)
      if (strcmp(dataIn.c_str(), "Agua 1") == 0) {
        Serial.println("Agua detectada");
        client.publish(WATER_TOPIC.c_str(), String(1).c_str());
      } else if (strcmp(dataIn.c_str(), "Agua 0") == 0) {
        Serial.println("Agua NO detectada");
        client.publish(WATER_TOPIC.c_str(), String(0).c_str());
      }
    }

    c = 0;
    dataIn = "";
    Serial.println("------");
  }
}
