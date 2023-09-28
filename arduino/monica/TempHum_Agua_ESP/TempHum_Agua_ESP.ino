#include <Wire.h>
#include "DHT.h"
#include <PubSubClient.h>

#if defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// Uncomment one of the lines below for whatever DHT AM2301_sensor type you're using!
#define DHTTYPE DHT21  // DHT 21 (AM2301)
uint8_t DHTPin = 14;   // D5 pin on nodeMCU board, GPIO pin 14 - connected DATA from AM2301_sensor

DHT dht(DHTPin, DHTTYPE);

//wifi
const char* ssid = "luisa";
const char* password = "123luisa";

//MQTT Server
const char* mqtt_server = "192.168.250.6";
const char* mqtt_username = "ucontrol";
const char* mqtt_password = "Ucontrol123";
const int mqtt_port = 1884;
// Time zone info
#define TZ_INFO "UTC+4"
const String TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Temperatura y humedad del laboratorio";

const String TOPIC_TEMPERATURE = TOPIC + " / Temperatura";


const String TOPIC_HUMIDITY = TOPIC + " / Humedad";



const String WATER_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Sensor de agua";


#define INFLUXDB_URL "http://192.168.250.6:8086"
#define INFLUXDB_TOKEN "6Yp7nch2M53gScyGE3H5RPyAu4F_c5auYRlryIKrDEEeG_YBx5RsKIByPP1NTAgABcoWRibtdWtrmy780mQXHg=="
#define INFLUXDB_ORG "b1c7cd22c3b5dd1e"
#define INFLUXDB_BUCKET "ucontrol"


float hum;   //Stores humidity value
float temp;  //Stores temperature value// Value for storing water level
int val = 0;
const int WATERPIN = A0;

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int waterValue = 0;



// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient influxClient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

Point waterSensor(WATER_TOPIC);
Point tempSensor(TOPIC_TEMPERATURE);
Point humSensor(TOPIC_HUMIDITY);
/**** Secure WiFi Connectivity Initialisation *****/
WiFiClient espClient;

/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

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
    if (client.connect(TOPIC.c_str(), mqtt_username, mqtt_password)) {
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

  // client.setServer(mqtt_server, mqtt_port);
  dht.begin();
}

void loop() {
  tempSensor.clearFields();
  humSensor.clearFields();
  waterSensor.clearFields();

  // if (!client.connected()) {
  //   reconnect();
  // }
  // client.loop();
  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  waterValue = analogRead(WATERPIN);  //put Sensor insert into soil
  Serial.println(waterValue);
  Serial.println(temp);
  Serial.println(hum);

  unsigned long now = millis();
  // if (now - lastMsg > 2000) {
  //   lastMsg = now;
  // client.publish(TOPIC_TEMPERATURE.c_str(), String(temp).c_str());
  // client.publish(TOPIC_HUMIDITY.c_str(), String(hum).c_str());

  tempSensor.addField("value", String(temp).c_str());
  humSensor.addField("value", String(hum).c_str());
  if (waterValue > 200) {
    waterSensor.addField("value", "1");
    Serial.println(waterSensor.toLineProtocol());
    // client.publish(WATER_TOPIC.c_str(), "1");
    if (!influxClient.writePoint(waterSensor)) {
      Serial.print("InfluxDB waterSensor write failed: ");
      Serial.println(influxClient.getLastErrorMessage());
    }
  }else{
     waterSensor.addField("value", "0");
    Serial.println(waterSensor.toLineProtocol());
    // client.publish(WATER_TOPIC.c_str(), "0");
    if (!influxClient.writePoint(waterSensor)) {
      Serial.print("InfluxDB waterSensor write failed: ");
      Serial.println(influxClient.getLastErrorMessage());
    }
  }
  // }

  Serial.print("Writing: ");
  Serial.println(tempSensor.toLineProtocol());
  Serial.println(humSensor.toLineProtocol());

  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Wifi connection lost");
  }

 
  if (!influxClient.writePoint(tempSensor)) {
    Serial.print("InfluxDB tempSensor write failed: ");
    Serial.println(influxClient.getLastErrorMessage());
  }
  if (!influxClient.writePoint(humSensor)) {
    Serial.print("InfluxDB humSensor write failed: ");
    Serial.println(influxClient.getLastErrorMessage());
  }
  delay(840000);
}
