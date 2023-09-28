#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>


//wifi
const char* ssid = "luisa";
const char* password = "123luisa";

// const char* ssid = "Cleopatra";
// const char* password = "11990587";


//MQTT Server
const char* mqtt_server = "192.168.250.6";
const char* mqtt_username = "ucontrol";
const char* mqtt_password = "Ucontrol123";
const int mqtt_port = 1884;

const String LIGHT_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Luces del laboratorio";
const String ACTION = LIGHT_TOPIC + " / Switch";


const String MOV_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Sensor de movimiento";
const String VIB_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Sensor de vibraciones";
const String SOIL_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Sensor de tierra para las macetas";


#define INFLUXDB_URL "http://192.168.250.6:8086"
#define INFLUXDB_TOKEN "6Yp7nch2M53gScyGE3H5RPyAu4F_c5auYRlryIKrDEEeG_YBx5RsKIByPP1NTAgABcoWRibtdWtrmy780mQXHg=="
#define INFLUXDB_ORG "b1c7cd22c3b5dd1e"
#define INFLUXDB_BUCKET "ucontrol"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient influxClient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

Point lightControl(LIGHT_TOPIC);
Point movControl(MOV_TOPIC);
Point vibControl(VIB_TOPIC);
Point soilControl(SOIL_TOPIC);

/**** Secure WiFi Connectivity Initialisation *****/
WiFiClient espClient;

/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

SoftwareSerial NodeMCU_SS(D1, D2);

// Time zone info
#define TZ_INFO "UTC+4"


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

void callback(char* LIGHT_TOPIC, byte* payload, unsigned int length) {
  String incommingMessage = "";

  for (int i = 0; i < length; i++) {
    incommingMessage += (char)payload[i];
  }
  Serial.println("Message arrived [" + String(LIGHT_TOPIC) + "]");

  //check incoming message
  //const char* topicName = strcat(LIGHT_TOPIC.c_str(),"SwitchControlESP");
  if (strcmp(LIGHT_TOPIC, ACTION.c_str()) == 0) {

    if (incommingMessage.equals("1")) {
      Serial.println("From broker: Bulb ON");
      String ack = "1" + String('\n');
      NodeMCU_SS.print(ack);
    } else if (incommingMessage.equals("0")) {
      Serial.println("From broker: Bulb OFF");
      String ack = "0" + String('\n');
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

    if (client.connect(LIGHT_TOPIC.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(LIGHT_TOPIC.c_str());
      client.subscribe(ACTION.c_str());

      client.subscribe(MOV_TOPIC.c_str());

      client.subscribe(VIB_TOPIC.c_str());

      client.subscribe(SOIL_TOPIC.c_str());

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
  client.setCallback(callback);

  delay(500);
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  if (influxClient.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(influxClient.getServerUrl());
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
    Serial.println(dataIn);
    unsigned long now = millis();
    if (now - lastMsg > 2000) {
      lastMsg = now;

      if ((strcmp(dataIn.c_str(), "Luz 1") != 0) && (strcmp(dataIn.c_str(), "Luz 0") != 0) && (strcmp(dataIn.c_str(), "Movimiento 1") != 0) && (strcmp(dataIn.c_str(), "Movimiento 0") != 0) && (strcmp(dataIn.c_str(), "Vibracion 1") != 0) && (strcmp(dataIn.c_str(), "Vibracion 0") != 0)) {
        Serial.print("Soil percent: ");
        soilControl.addField("value", dataIn.toInt());
        Serial.print("Writing: ");
        Serial.println(soilControl.toLineProtocol());
        if (!influxClient.writePoint(soilControl)) {
          Serial.print("InfluxDB soilControl write failed: ");
          Serial.println(influxClient.getLastErrorMessage());
        }
      }

      if (strcmp(dataIn.c_str(), "Luz 1") == 0) {
        Serial.println("Light detected!");  // Verifica si contiene "Luz1"
        lightControl.addField("value", 1);
        Serial.print("Writing: ");
        Serial.println(lightControl.toLineProtocol());
        if (!influxClient.writePoint(lightControl)) {
          Serial.print("InfluxDB lightControl write failed: ");
          Serial.println(influxClient.getLastErrorMessage());
        }
      }

      if (strcmp(dataIn.c_str(), "Luz 0") == 0) {
        Serial.println("No light detected!");  // Verifica si contiene "Luz0"
        lightControl.addField("value", 0);
        Serial.print("Writing: ");
        Serial.println(lightControl.toLineProtocol());
        if (!influxClient.writePoint(lightControl)) {
          Serial.print("InfluxDB lightControl write failed: ");
          Serial.println(influxClient.getLastErrorMessage());
        }
      }
      if (strcmp(dataIn.c_str(), "Movimiento 1") == 0) {
        Serial.println("Motion detected!");
        // client.publish(MOV_TOPIC.c_str(), "1");
        movControl.addField("value", 1);

        Serial.print("Writing: ");
        Serial.println(movControl.toLineProtocol());
        if (!influxClient.writePoint(movControl)) {
          Serial.print("InfluxDB movControl write failed: ");
          Serial.println(influxClient.getLastErrorMessage());
        }
      }
      if (strcmp(dataIn.c_str(), "Vibracion 1") == 0) {
        Serial.println("Vibracion detectada");  // Verifica si contiene "Vibracion1"
        // client.publish(VIB_TOPIC.c_str(), "1");
        vibControl.addField("value", 1);
        
        Serial.print("Writing: ");
        Serial.println(vibControl.toLineProtocol());
        if (!influxClient.writePoint(vibControl)) {
          Serial.print("InfluxDB vibControl write failed: ");
          Serial.println(influxClient.getLastErrorMessage());
        }
      } else if (strcmp(dataIn.c_str(), "Vibracion 0") == 0) {  // Verifica si contiene "Vibracion0"
        // client.publish(VIB_TOPIC.c_str(), 0);
        vibControl.addField("value", 0);
        
        Serial.print("Writing: ");
        Serial.println(vibControl.toLineProtocol());
        if (!influxClient.writePoint(vibControl)) {
          Serial.print("InfluxDB vibControl write failed: ");
          Serial.println(influxClient.getLastErrorMessage());
        }
      }
    }



    c = 0;
    dataIn = "";
  }
}
