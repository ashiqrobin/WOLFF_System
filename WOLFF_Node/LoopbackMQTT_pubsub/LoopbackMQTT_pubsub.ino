/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Update these with values suitable for your network.

/*
 * Specify the credentials for he access point to connect to and the mqtt broker
 */
const char* ssid = "ESP_Access_Point_2";
const char* password = "helloworld";
const char* mqtt_server = "192.168.4.1";

/*
 * Setup the esp client
 */
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);         // Initialize the BUILTIN_LED pin as an output
  /*
   * Begin serial comm and set callbacks
   */
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
/*
 * Connect to wifi
 */
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
/*
 * Print wifi access point info once connected
 */
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  /*
   * Whenever a message is received print it out and publish an ACK
   */
  static int ledState = 0;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.println("] ");
  Serial.println((String)"Message length: " + length);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  char* message = (char*) payload;
  Serial.println(message);

  client.publish("ack_messages", "ACK");

  digitalWrite(BUILTIN_LED, ledState);   // Toggle the LED 
  ledState = 1 - ledState;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print((String)"Attempting MQTT connection to " + mqtt_server + "...");
    // Attempt to connect
    // Create a random client ID
    String clientId = "ESP8266Client--";
    clientId += String(random(0xffff), HEX);
    Serial.println(clientId);

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected to broker");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("esp_messages");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
/*
 * Keep trying to reconnect to the MQTT broker if unsuccessful
 */
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
