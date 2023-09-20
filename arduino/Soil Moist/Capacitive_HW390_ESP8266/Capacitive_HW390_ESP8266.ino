#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#if defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#endif

// wifi
const char* ssid = "luisa";
const char* password = "123luisa";
// const char* ssid = "Cleopatra";
// const char* password = "11990587";

// //MQTT Server
// const char* mqtt_server = "25.58.78.34";
// const char* mqtt_username = "ucontrol";
// const char* mqtt_password = "Ucontrol123";
// const int mqtt_port = 1884;


//MQTT Server
const char* mqtt_server = "192.168.152.71";
const char* mqtt_username = "ucontrol";
const char* mqtt_password = "Ucontrol123";
const int mqtt_port = 1884;

const String HW_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Sensor de tierra";

/**** Secure WiFi Connectivity Initialisation *****/
WiFiClient espClient;

/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


const int AirValue = 850;
const int WaterValue = 20;
int intervals = (AirValue - WaterValue) / 3;
const int SensorPin = A0;
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

    if (client.connect(HW_TOPIC.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(HW_TOPIC.c_str());
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

}


void loop() {

 if (!client.connected()) {
    reconnect();
  }
  client.loop();
    //Read data and store it
    soilMoistureValue = analogRead(SensorPin);  //put Sensor insert into soil
    soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
    Serial.println(soilmoisturepercent);


    //Broker
    unsigned long now = millis();
    if (now - lastMsg > 2000) {
      lastMsg = now;
       client.publish(HW_TOPIC.c_str(), String(soilmoisturepercent).c_str());
       Serial.println("enviado");
    }
  delay(30000);
}
