#include <SoftwareSerial.h>;

#include <ESP8266WiFi.h>;
#include <PubSubClient.h>;
#include <time.h>;
#include <TZ.h>;
#include <FS.h>;
#include <LittleFS.h>;
#include <CertStoreBearSSL.h>;
#include <ArduinoJson.h>;



//wifi
const char* ssid = "luisa";
const char* password = "123luisa";

//MQTT Server
const char* mqtt_server = "6f73b3569de64e8c94164637cdc7a301.s2.eu.hivemq.cloud";
const char* topicT = "channels/2207179/publish/fields/field1";
const char* topicH = "channels/2207179/publish/fields/field2";
const char* mqtt_clientid = "ucontrol";
const char* mqtt_username = "ucontrol";
const char* mqtt_password = "Ucontrol123";
const int mqtt_port = 8883;
const int mqtt_websocket_port = 8884;
long writeChannelID = 86;

/**** Secure WiFi Connectivity Initialisation *****/
WiFiClientSecure espClient;

/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value=0;

static const char* root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";


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


void setDateTime() {
  // You can use your own timezone, but the exact time is not used at all.
  // Only the date is needed for validating the certificates.
  configTime(TZ_America_Caracas, "pool.ntp.org", "time.google.com");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(1000);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();

  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.printf("%s %s", tzname[0], asctime(&timeinfo));
}

/***** Call back Method for Receiving MQTT messages *****/

void callback(char* topic, byte* payload, unsigned int length) {


  String incommingMessage = "";

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    incommingMessage += (char)payload[i];
  }
  Serial.println("Message arrived [" + String(topic) + "]" + incommingMessage);

  //check incoming message
  if (strcmp(topic, "Bombillo/Lab. Prototipos") == 0) {
    if (incommingMessage.equals("ON")) {
      Serial.println("From broker: Bulb ON");
    } else if (incommingMessage.equals("OFF")) {
      Serial.println("From broker: Bulb OFF");
    }
  } else {
    Serial.println("Nothing");
  }
}


void reconnect() {
  // Loop until we’re reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection…");
    String clientId = mqtt_clientid;
    // Attempt to connect
    // Insert your password
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Once connected, subscribe the topic
      client.subscribe("Bombillo/Lab. Prototipos");


    } else {
      Serial.print("failed, rc = ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


/**** Method for Publishing MQTT Messages **********/
void publishMessage(const char* topic, String payload, boolean retained) {
  if (client.publish(topic, payload.c_str(), true))
    Serial.println("Message publised [" + String(topic) + "]: " + payload);
}

void setup() {
  Serial.begin(9600);


  delay(500);

  LittleFS.begin();
  setup_wifi();
#ifdef ESP8266
  espClient.setInsecure();
#else
  espClient.setCACert(root_ca);  // enable this line and the the "certificate" code for secure connection
#endif

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
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

    // if (!client.connected()) {
    //   reconnect();
    // }
    // client.loop();

   
  }
  //  //To serial monitor
  //    if (c == '\n') {
  //     String ack=String("Recibido")+String('\n');
  //     NodeMCU_SS.print(ack);
  //     Serial.println(dataIn);
  //     c = 0;
  //     dataIn = "";
  //   }


    // To Broker
    if (c == '\n') {
      Serial.println(dataIn);
      String ack = String("Recibido") + String('\n');
      NodeMCU_SS.print(ack);

      DynamicJsonDocument doc(1024);

      doc["deviceId"] = "NodeMCU";
      doc["siteId"] = "My Demo Lab";
      doc["bulbState"] = dataIn;

      char mqtt_message[128];
      serializeJson(doc, mqtt_message);

    Serial.println(mqtt_message);
     publishMessage("Bombillo/Lab. Prototipos", mqtt_message, true);

      // unsigned long now = millis();
      // if (now - lastMsg > 2000) {
      //   lastMsg = now;
      //   // ++value;
      //   // snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
      //   // Serial.print("Publish message: ");
      //   // Serial.println(msg);
      //     Serial.println("Publicando...");

      //   static char s_dataIn[50];
      //   client.publish("Bombillo/Lab. Prototipos", dataIn.c_str());
      // }
      c = 0;
      dataIn = "";
    }
   
}
