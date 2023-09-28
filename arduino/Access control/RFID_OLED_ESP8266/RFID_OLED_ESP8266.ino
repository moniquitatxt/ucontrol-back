#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/*
In the ESP8266, D3 pin is RST_PIN and
D8 pin is SS_PIN
*/
#define RST_PIN D0
#define SS_PIN D8

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET -1  // Reset pin
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// //wifi
const char* ssid = "luisa";
const char* password = "123luisa";

// //MQTT Server
// const char* mqtt_server = "172.29.91.241";
// const char* mqtt_clientid = "pato";
// const char* mqtt_username = "ucontrol";
// const char* mqtt_password = "Ucontrol123";
// const int mqtt_port = 1884;

// const char* ssid = "Cleopatra";
// const char* password = "11990587";

// //MQTT Server
// const char* mqtt_server = "25.58.78.34";
// const char* mqtt_username = "ucontrol";
// const char* mqtt_password = "Ucontrol123";
// const int mqtt_port = 1884;


//MQTT Server
const char* mqtt_server = "192.168.250.6";
const char* mqtt_username = "ucontrol";
const char* mqtt_password = "Ucontrol123";
const int mqtt_port = 1884;

const String TOPIC = "Escuela de Ingeniería Civil / Laboratorio de ingenieria sanitaria / Control de acceso del laboratorio";
const String ACTION = TOPIC + " / Permiso";

/**** Secure WiFi Connectivity Initialisation *****/
WiFiClient espClient;

/**** MQTT Client Initialisation Using WiFi Connection *****/
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;

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



void callback(char* topic, byte* payload, unsigned int length) {
  String incommingMessage = "";

  for (int i = 0; i < length; i++) {
    incommingMessage += (char)payload[i];
  }
  Serial.println("Message arrived [" + String(topic) + "]");

  //check incoming message
  //const char* topicName = strcat(TOPIC.c_str(),"SwitchControlESP");
  if (strcmp(topic, ACTION.c_str()) == 0) {

      Serial.println(incommingMessage);
      display.setTextColor(WHITE);
      display.setCursor(0, 30);
      display.println(incommingMessage);
      display.display();
      display.clearDisplay();
    
  } else {
    Serial.println("Nothing");
  }
}


/************* Connect to MQTT Broker ***********/
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect("AccessControlESP", mqtt_username, mqtt_password)) {
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
  Serial.begin(9600);  // Initialize serial communications and wait until it is ready
  while (!Serial) {
    // Nothing here. Just wait for serial to be present
  }


  setup_wifi();


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


  if (!client.connected()) {
    reconnect();
  }
  client.loop();


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


  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;


    client.publish(TOPIC.c_str(), serial.c_str());
  }

  // Halt PICC
  reader.PICC_HaltA();
  // Stop encryption on PCD
  reader.PCD_StopCrypto1();
}