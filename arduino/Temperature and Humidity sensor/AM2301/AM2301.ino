#include <Wire.h>
#include "DHT.h"
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#if defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#endif

// Uncomment one of the lines below for whatever DHT AM2301_sensor type you're using!
#define DHTTYPE DHT21  // DHT 21 (AM2301)
uint8_t DHTPin = 14;   // D5 pin on nodeMCU board, GPIO pin 14 - connected DATA from AM2301_sensor

DHT dht(DHTPin, DHTTYPE);
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

const String AM2301_TOPIC = "Escuela de Ingeniería Civil / Oficina Profe Yolanda / Temperatura y humedad de la oficina";

// Declare Data point
Point AM2301_sensor(AM2301_TOPIC);

float hum;   //Stores humidity value
float temp;  //Stores temperature value



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


void setup() {
  Serial.begin(9600);
  setup_wifi();
  // Accurate time is necessary for certificate validation and writing in batches
  // We use the NTP servers in your area as provided by: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  delay(500);
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  if (influxClient.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(influxClient.getServerUrl());
    AM2301_sensor.addTag("deviceType", "tempHum");
    pinMode(DHTPin, INPUT);
    dht.begin();
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(influxClient.getLastErrorMessage());
  }
}

void loop() {
  // Clear fields for reusing the point. Tags will remain the same as set above.
  if (influxClient.validateConnection()) {
    AM2301_sensor.clearFields();


    //Read data and store it to variables hum and temp
    hum = dht.readHumidity();
    temp = dht.readTemperature();

    // Store measured value into point
    AM2301_sensor.addField("temperature", temp);
    AM2301_sensor.addField("humidity", hum);

    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(AM2301_sensor.toLineProtocol());
    // Check WiFi connection and reconnect if needed
    if (wifiMulti.run() != WL_CONNECTED) {
      Serial.println("Wifi connection lost");
    }

    // Write point
    if (!influxClient.writePoint(AM2301_sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(influxClient.getLastErrorMessage());
    }

  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(influxClient.getLastErrorMessage());
  }

  delay(30000);
}
