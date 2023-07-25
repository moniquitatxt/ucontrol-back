#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <LittleFS.h>
#include <ArduinoJson.h>

/*
In the ESP8266, D3 pin is RST_PIN and
D4 pin is SS_PIN
*/
#define RST_PIN D0
#define SS_PIN D8

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1  // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// wifi
const char* ssid = "luisa";
const char* password = "123luisa";

//MQTT Server
const char* mqtt_server = "6f73b3569de64e8c94164637cdc7a301.s2.eu.hivemq.cloud";
const char* topicAccess = "Puerta/Lab. Prototipos/Escuela Ing. Informatica";
const char* mqtt_clientid = "ucontrol";
const char* mqtt_username = "ucontrol";
const char* mqtt_password = "Ucontrol123";
const int mqtt_port = 8883;
const int mqtt_websocket_port = 8884;
long writeChannelID = 86;
const String NAMES_SEPARATOR = "-";


const String DEVICE_NAME = "Acceso" + NAMES_SEPARATOR + topicAccess;

/**** Secure WiFi Connectivity Initialisation *****/
WiFiClientSecure espClient;

/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

/****** root certificate *********/

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


MFRC522 reader(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;






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


/***** Call back Method for Receiving MQTT messages *****/

void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";

  String action = "";

  for (int i = 0; i < length; i++) {
    incommingMessage += (char)payload[i];
  }
   Serial.println("Message arrived [" + String(topic) + "]: " + incommingMessage);

  //check incoming message
  if (strcmp(topic, topicAccess) == 0) {
    if (incommingMessage.equals("ON")) {
      //Send back to arduino action to turn on bulb

      NodeMCU_SS.print(String("ON") + String('\n'));

    } else if (incommingMessage.equals("OFF")) {
      //Send back to arduino action to turn off bulb
      NodeMCU_SS.print(String("OFF") + String('\n'));
    }
  } else {
    Serial.println("Nothing");
  }
}


/************* Connect to MQTT Broker ***********/
void reconnect(const char* topic) {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";  // Create a random client ID
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");

      client.subscribe(topic);  // subscribe the topics here

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");  // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/**** Method for Publishing MQTT Messages **********/
void publishMessage(const char* topic, String payload, boolean retained) {
  if (client.publish(topic, payload.c_str(), true)) {
    Serial.println("Message published [" + String(topic) + "] " + payload);
  }
}


void setup() {
  Serial.begin(9600);  // Initialize serial communications and wait until it is ready
  while (!Serial) {
    // Nothing here. Just wait for serial to be present
  }

  delay(500);

  LittleFS.begin();
  setup_wifi();
  // setDateTime();
#ifdef ESP8266
  espClient.setInsecure();
#else
  espClient.setCACert(root_ca);  // enable this line and the the "certificate" code for secure connection
#endif

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  SPI.begin();

  reader.PCD_Init();
  // Just wait some seconds...
  delay(4);
  // Prepare the security key for the read and write functions.
  // Normally it is 0xFFFFFFFFFFFF
  // Note: 6 comes from MF_KEY_SIZE in MFRC522.h
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;  //keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library
  }
  Serial.println("Ready!");

  // initialize the OLED object
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Clear the buffer.
  display.clearDisplay();

  // Display Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Codigo de carnets UCAB");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!reader.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards. This returns false if read is not successful; and if that happens, we stop the code
  if (!reader.PICC_ReadCardSerial()) {
    return;
  }

  // At this point, the serial can be read. We transform from byte to hex

  String serial = "";
  for (int x = 0; x < reader.uid.size; x++) {
    // If it is less than 10, we add zero
    if (reader.uid.uidByte[x] < 0x10) {
      serial += "0";
    }
    // Transform the byte to hex
    serial += String(reader.uid.uidByte[x], HEX);
  }
  // Transform to uppercase
  serial.toUpperCase();
  serial.concat('\n');

  if (!client.connected()) {
    reconnect(topicAccess);
  }
  client.loop();


  Serial.println("Read serial is: " + serial);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Codigo: ");
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  display.println(serial);
  display.display();
  display.clearDisplay();


  DynamicJsonDocument doc(1024);

  doc["deviceId"] = DEVICE_NAME;
  doc["siteId"] = "UControl";
  doc["cardCode"] = serial;


  char mqtt_message[128];
  serializeJson(doc, mqtt_message);

  publishMessage(topicAccess, mqtt_message, true);


  // Halt PICC
  reader.PICC_HaltA();
  // Stop encryption on PCD
  reader.PCD_StopCrypto1();
}