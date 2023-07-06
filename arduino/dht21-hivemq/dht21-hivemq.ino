#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include <TZ.h>
#include <FS.h>
#include <LittleFS.h>
#include <CertStoreBearSSL.h>
#include <DHT.h>  // DHT library 1.2.3 (newer library returns error)

#define DHTTYPE DHT21  // dht type of sensor, change if you have other
#define DHTPIN 14      // D5 pin on nodeMCU board, GPIO pin 14 - connected DATA from sensor

// wifi
const char* ssid = "luisa";
const char* password = "123luisa";

// MQTT Server
const char* mqtt_server = "17363ff3932e49388d23181ed491867a.s2.eu.hivemq.cloud";
const char* topicT = "channels/2207179/publish/fields/field1";
const char* topicH = "channels/2207179/publish/fields/field2";
const char* mqtt_clientid = "luisamlopez";
const char* mqtt_username = "luisamlopez";
const char* mqtt_password = "279480412.Lm";
const int mqtt_port = 8883;
const int mqtt_websocket_port = 8883;
long writeChannelID = 86;

// A single, global CertStore which can be used by all connections.
// Needs to stay live the entire time any of the WiFiClientBearSSLs
// are present.
BearSSL::CertStore certStore;

WiFiClientSecure espClient;
PubSubClient* client;
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];
int value = 0;


// Sensor
DHT dht(DHTPIN, DHTTYPE);  // declare dht object to access sensor

float hum;   //Stores humidity value
float temp;  //Stores temperature value

// Temporary variables
static char s_temp[7];
static char s_hum[7];


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
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
    delay(100);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println();

  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.printf("%s %s", tzname[0], asctime(&timeinfo));
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if the first character is present
  if ((char)payload[0] != NULL) {
    //digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
    Serial.println("The fist character is present");
    delay(500);
    Serial.println("The fist character is NOT present ANYMORE");
    // digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  } else {
    Serial.println("The fist character is NOT present");
    // digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  }
}


void reconnect() {
  // Loop until we’re reconnected
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection…");
    String clientId = mqtt_clientid;
    // Attempt to connect
    // Insert your password
    if (client->connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement…
      client->publish("testTopic", "hello world");
      // … and resubscribe
      client->subscribe("testTopic");
    } else {
      Serial.print("failed, rc = ");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  delay(500);
  // When opening the Serial Monitor, select 9600 Baud
  Serial.begin(9600);
  delay(500);

  LittleFS.begin();
  setup_wifi();
  setDateTime();

  // you can use the insecure mode, when you want to avoid the certificates

 // espClient->setInsecure();

  // int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  // Serial.printf("Number of CA certs read: %d\n", numCerts);
  // if (numCerts == 0) {
  //   Serial.printf("No certs found. Did you run certs-from-mozilla.py and upload the LittleFS directory before running?\n");
  //   return; // Can't connect to anything w/o certs!
  // }

  // BearSSL::WiFiClientSecure *bear = new BearSSL::WiFiClientSecure();
  // // Integrate the cert store with this connection
  // bear->setCertStore(&certStore);

  // client = new PubSubClient(*bear);

  client->setServer(mqtt_server, mqtt_port);
  client->setCallback(callback);

  dht.begin();
}

void loop() {
  if (!client->connected()) {
    reconnect();
  }
  client->loop();


  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  temp = dht.readTemperature();

  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print("%,  Temperature: ");
  Serial.print(temp);
  Serial.println(" Celsius");

  // convertir un número de punto flotante (float) en una cadena de caracteres (string)
  dtostrf(temp, 6, 2, s_temp);
  dtostrf(hum, 6, 2, s_hum);

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    snprintf(msg, MSG_BUFFER_SIZE, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client->publish("Temperatura Lab. Prototipos", s_temp); 
    client->publish("Humedad Lab. Prototipos", s_hum);
  }
}
