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

const String TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Luces del laboratorio";
const String ACTION = TOPIC + " / Switch";


const String MOV_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Sensor de movimiento";
const String VIB_TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Sensor de vibraciones";

/**** Secure WiFi Connectivity Initialisation *****/
WiFiClient espClient;

/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

SoftwareSerial NodeMCU_SS(D1, D2);


#define INFLUXDB_URL "http://25.65.250.117:8086/"
#define INFLUXDB_TOKEN "Fe_oRB6aae5GylUjfUdrSUaXh2ZNsJ4TuaqybEcQRk9EMabi1JqhdqiqizzeVLOh1DfQc2BR_Ig_eRBTq6p9hg=="
#define INFLUXDB_ORG "0c699b31e8577998"
#define INFLUXDB_BUCKET "ucontrol"

// Time zone info
#define TZ_INFO "UTC+4"
InfluxDBClient influxClient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);


Point lightControl(TOPIC);

Point movControl(MOV_TOPIC);

Point vibControl(VIB_TOPIC);

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

    if (client.connect(TOPIC.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(TOPIC.c_str());
      client.subscribe(ACTION.c_str());

      client.subscribe(MOV_TOPIC.c_str());

      client.subscribe(VIB_TOPIC.c_str());

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

    if (influxClient.validateConnection()) {
      Serial.print("Connected to InfluxDB: ");
      Serial.println(influxClient.getServerUrl());
      unsigned long now = millis();
      if (now - lastMsg > 2000) {
        lastMsg = now;
        if (strcmp(dataIn.c_str(), "Luz 1") == 0) {
          Serial.println("Light detected!");  // Verifica si contiene "Luz1"
          client.publish(TOPIC.c_str(), "1");
          lightControl.addField("value", "1");
        }

        if (strcmp(dataIn.c_str(), "Luz 0") == 0) {

          Serial.println("No light detected!");  // Verifica si contiene "Luz0"
          client.publish(TOPIC.c_str(), "0");
          lightControl.addField("value", "0");
        }
        if (strcmp(dataIn.c_str(), "Movimiento 1") == 0) {
          Serial.println("Motion detected!");
          client.publish(MOV_TOPIC.c_str(), "1");
          movControl.addField("value", "0");
        } else if (strcmp(dataIn.c_str(), "Movimiento 0") == 0) {  // Verifica si contiene "Movimiento0"
          client.publish(MOV_TOPIC.c_str(), "0");
          movControl.addField("value", "0");
        }
        if (strcmp(dataIn.c_str(), "Vibracion 1") == 0) {
          Serial.println("Vibracion detectada");  // Verifica si contiene "Vibracion1"
          client.publish(VIB_TOPIC.c_str(), "1");
          vibControl.addField("value", "0");
        } else if (strcmp(dataIn.c_str(), "Vibracion 0") == 0) {  // Verifica si contiene "Vibracion0"
          client.publish(VIB_TOPIC.c_str(), "0");
          vibControl.addField("value", "0");
        }

        if (!influxClient.writePoint(lightControl)) {
          Serial.print("InfluxDB write failed: ");
          Serial.println(influxClient.getLastErrorMessage());
        }
        if (!influxClient.writePoint(vibControl)) {
          Serial.print("InfluxDB write failed: ");
          Serial.println(influxClient.getLastErrorMessage());
        }
        if (!influxClient.writePoint(movControl)) {
          Serial.print("InfluxDB write failed: ");
          Serial.println(influxClient.getLastErrorMessage());
        }
      }
    } else {
      Serial.print("InfluxDB connection failed: ");
      Serial.println(influxClient.getLastErrorMessage());
    }


    c = 0;
    dataIn = "";
  }
}
