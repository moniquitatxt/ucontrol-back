#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <ArduinoJson.h>
#include <DHT.h>


#define DHTTYPE DHT21  // dht type of sensor, change if you have other
#define DHTPIN 14      // D5 pin on nodeMCU board, GPIO pin 14 - connected DATA from sensor


// wifi
const char* ssid = "luisa";
const char* password = "123luisa";

// MQTT Server
const char* mqtt_server = "172.18.20.241";

const char* mqtt_username = "ucontrol";
const char* mqtt_password = "Ucontrol123";
const int mqtt_port = 1884;

const String TOPIC_T = "Escuela de Ingeniería Civil / Oficina Profe Yolanda / Sensor de Temperatura";
const String TOPIC_H = "Escuela de Ingeniería Civil Oficina Profe Yolanda / Sensor de Humedad";

/**** Secure WiFi Connectivity Initialisation *****/
WiFiClient espClient;

/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


// Sensor
DHT dht(DHTPIN, DHTTYPE);  // declare dht object to access sensor

float hum;   //Stores humidity value
float temp;  //Stores temperature value


/************* Connect to WiFi ***********/
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
/************* Connect to MQTT Broker ***********/
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("AM2301_ESP", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(TOPIC_T.c_str());
      client.subscribe(TOPIC_H.c_str());
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

  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print("%,  Temperature: ");
  Serial.print(temp);
  Serial.println(" Celsius");

  DynamicJsonDocument doc(1024);

  doc["deviceId"] = "NodeMCU";
  doc["siteId"] = "My Demo Lab";
  doc["humidity"] = hum;
  doc["temperature"] = temp;

  char mqtt_message[128];
  serializeJson(doc, mqtt_message);

  unsigned long now = millis();
  if (now - lastMsg > 100) {
    lastMsg = now;
    client.publish(TOPIC_T.c_str(), String(temp).c_str());
    client.publish(TOPIC_H.c_str(), String(hum).c_str());
  }
  delay(5000);
}
