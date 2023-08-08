#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h> // DHT library 1.2.3 (newer library returns error)

#define DHTTYPE DHT21 // dht type of sensor, change if you have other
#define DHTPIN 14 // D5 pin on nodeMCU board, GPIO pin 14 - connected DATA from sensor 

// wifi
const char* ssid = "luisa";
const char* password = "123luisa";

// MQTT Broker
const char *mqtt_server = "mqtt3.thingspeak.com";
const char *topicT = "channels/2207179/publish/fields/field1";
const char *topicH = "channels/2207179/publish/fields/field2";
const char *mqtt_clientid = "EwkcAwUzDh8xCCkQAAgpMhU";
const char *mqtt_username = "EwkcAwUzDh8xCCkQAAgpMhU";
const char *mqtt_password = "8mvGHbbh+JVVjRHjqpytb3rD";
const int   mqtt_port = 1883;
long writeChannelID = 86;

WiFiClient espClient;
PubSubClient client(espClient);

// Sensor
DHT dht(DHTPIN, DHTTYPE); // declare dht object to access sensor

float hum;  //Stores humidity value
float temp; //Stores temperature value

// Temporary variables
static char s_temp[7];
static char s_hum[7];

void setup_wifi() {

  delay(10);
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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_clientid,mqtt_username,mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
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
  temp= dht.readTemperature();
  
  //Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print("%,  Temperature: ");
  Serial.print(temp);
  Serial.println(" Celsius");

  dtostrf(temp, 6, 2, s_temp);
  dtostrf(hum, 6, 2, s_hum);
  
  client.publish(topicT,  s_temp );
  client.publish(topicH,  s_hum );
  
  delay(2000); //Delay 2 sec.
}