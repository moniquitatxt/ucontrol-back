#include <Wire.h>
#include "DHT.h"
#include <PubSubClient.h>

#if defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#endif

// Uncomment one of the lines below for whatever DHT AM2301_sensor type you're using!
#define DHTTYPE DHT21  // DHT 21 (AM2301)
uint8_t DHTPin = 14;   // D5 pin on nodeMCU board, GPIO pin 14 - connected DATA from AM2301_sensor

DHT dht(DHTPin, DHTTYPE);

//wifi
const char* ssid = "luisa";
const char* password = "123luisa";

//MQTT Server
const char* mqtt_server = "192.168.152.71";
const char* mqtt_username = "ucontrol";
const char* mqtt_password = "Ucontrol123";
const int mqtt_port = 1884;
// Time zone info
#define TZ_INFO "UTC+4"
const String TOPIC ="Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Temperatura y humedad del laboratorio";

const String TOPIC_TEMPERATURE = TOPIC+" / Temperatura";


const String TOPIC_HUMIDITY = TOPIC +" / Humedad";


float hum;   //Stores humidity value
float temp;  //Stores temperature value

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


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
  dht.begin();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  Serial.println(temp);
  Serial.println(hum);

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    client.publish(TOPIC_TEMPERATURE.c_str(), String(temp).c_str());
    client.publish(TOPIC_HUMIDITY.c_str(), String(hum).c_str());
  }
  delay(30000);
}
