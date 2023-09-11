#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

//wifi
const char* ssid = "luisa";
const char* password = "123luisa";

//MQTT Server
const char* mqtt_server = "172.29.91.241";
const char* mqtt_clientid = "pato";
const char* mqtt_username = "ucontrol";
const char* mqtt_password = "Ucontrol123";
const int mqtt_port = 1884;

const String TOPIC = "Escuela de Ingeniería Civil / Oficina Profe Yolanda / Bombillo de la oficina";
const String ACTION = "Escuela de Ingeniería Civil / Oficina Profe Yolanda / Bombillo de la oficina / Switch";

#define INFLUXDB_URL "http://172.29.91.241:8086"
#define INFLUXDB_TOKEN "oaz4hK-TQdb-5nBCuXs6zQCVa1uAn_QgIAeztBFJOWDx5rJVZ69zXKSU4ova8ShYRNNSf3QJShnsx5aVIcDI3Q=="
#define INFLUXDB_ORG "1bbe5f3a949fb99b"
#define INFLUXDB_BUCKET "ucontrol-arm21"

// Time zone info
#define TZ_INFO "UTC+4"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient influxClient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);


// Declare Data point
Point relayControl(TOPIC);

/**** Secure WiFi Connectivity Initialisation *****/
WiFiClient espClient;

/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

SoftwareSerial NodeMCU_SS(D1, D2);

char c;
String dataIn;

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

/***** Call back Method for Receiving MQTT messages *****/

void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";

  for (int i = 0; i < length; i++) {
    incommingMessage += (char)payload[i];
  }
  Serial.println("Message arrived [" + String(topic) + "]");

  //check incoming message
  //const char* topicName = strcat(TOPIC.c_str(),"SwitchControlESP");
  if (strcmp(topic, ACTION.c_str()) == 0) {

    if (incommingMessage.equals("1")) {
      Serial.println("From broker: Bulb ON");
      String ack = String(1) + String('\n');
      NodeMCU_SS.print(ack);
    } else if (incommingMessage.equals("0")) {
      Serial.println("From broker: Bulb OFF");
      String ack = String(0) + String('\n');
      NodeMCU_SS.print(ack);
    }
  } else {
    Serial.println("Nothing");
  }
}


/************* Connect to MQTT Broker ***********/
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(TOPIC.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(TOPIC.c_str());

      client.subscribe(ACTION.c_str());
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


  // delay(500);

  // LittleFS.begin();
  setup_wifi();


  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  delay(500);
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  if (influxClient.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(influxClient.getServerUrl());
    relayControl.addTag("deviceType", "luz");
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(influxClient.getLastErrorMessage());
  }

  NodeMCU_SS.begin(115200);
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


  // To Broker
  if (c == '\n') {
    relayControl.clearFields();

    unsigned long now = millis();
    if (now - lastMsg > 2000) {
      lastMsg = now;

      if (dataIn == "1") {
        client.publish(TOPIC.c_str(), "1");
        relayControl.addField("switchStatus", "1");
      } else if (dataIn == "0") {
        client.publish(TOPIC.c_str(), "0");
        relayControl.addField("switchStatus", "0");
      }

      Serial.println(relayControl.toLineProtocol());
      if (!influxClient.writePoint(relayControl)) {
        Serial.print("InfluxDB write failed: ");
        Serial.println(influxClient.getLastErrorMessage());
      }
    }
    c = 0;
    dataIn = "";
  }
}
