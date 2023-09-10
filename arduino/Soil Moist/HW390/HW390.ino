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

#define INFLUXDB_URL "http://172.29.91.241:8086"
#define INFLUXDB_TOKEN "oaz4hK-TQdb-5nBCuXs6zQCVa1uAn_QgIAeztBFJOWDx5rJVZ69zXKSU4ova8ShYRNNSf3QJShnsx5aVIcDI3Q=="
#define INFLUXDB_ORG "1bbe5f3a949fb99b"
#define INFLUXDB_BUCKET "ucontrol-arm21"

// Time zone info
#define TZ_INFO "UTC+4"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient influxClient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);


//MQTT Server
const char* mqtt_server = "25.58.78.34";
const char* mqtt_username = "ucontrol";
const char* mqtt_password = "Ucontrol123";
const int mqtt_port = 1884;

const String HW_TOPIC = "Escuela de Ingeniería Civil / Oficina Profe Yolanda / Sensor de macetas";

// Declare Data point
Point HW_sensor(HW_TOPIC);

/**** Secure WiFi Connectivity Initialisation *****/
WiFiClient espClient;

/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;


const int AirValue = 950;
const int WaterValue = 450;
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

    if (client.connect("HW390_ESP", mqtt_username, mqtt_password)) {
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

  // client.setServer(mqtt_server, mqtt_port);
  // client.setCallback(callback);
  delay(500);
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  if (influxClient.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(influxClient.getServerUrl());
    HW_sensor.addTag("deviceType", "hum");
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(influxClient.getLastErrorMessage());
  }
}


void loop() {

  if (influxClient.validateConnection()) {
    HW_sensor.clearFields();


    //Read data and store it
    soilMoistureValue = analogRead(SensorPin);  //put Sensor insert into soil
    soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
    Serial.println(soilmoisturepercent);


      HW_sensor.addField("soilValue", soilmoisturepercent);

    // if (!client.connected()) {
    //   reconnect();
    // }
    // client.loop();


    //Broker
    unsigned long now = millis();
    // if (now - lastMsg > 2000) {
    //   lastMsg = now;

    if (soilMoistureValue < WaterValue) {

      //  client.publish(HW_TOPIC.c_str(), "Inundación");
      HW_sensor.addField("soilState", "Inundación");

    } else if (soilMoistureValue >= WaterValue && soilMoistureValue < AirValue) {

      //   client.publish(HW_TOPIC.c_str(), "Normal");
      HW_sensor.addField("soilState", "Normal");
    } else {

      //   client.publish(HW_TOPIC.c_str(), "Seca");
      HW_sensor.addField("soilState", "Seca");
    }
    //  }



    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(HW_sensor.toLineProtocol());
    // Check WiFi connection and reconnect if needed
    if (wifiMulti.run() != WL_CONNECTED) {
      Serial.println("Wifi connection lost");
    }

    // Write point
    if (!influxClient.writePoint(HW_sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(influxClient.getLastErrorMessage());
    }

  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(influxClient.getLastErrorMessage());
  }


  delay(30000);
}
