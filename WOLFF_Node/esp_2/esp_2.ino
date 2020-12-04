/*
 * uMQTTBroker demo for Arduino (C++-style)
 * 
 * The program defines a custom broker class with callbacks, 
 * starts it, subscribes locally to anything, and publishs a topic every second.
 * Try to connect from a remote client and publish something - the console will show this as well.
 */

#include <ESP8266WiFi.h>
#include <uMQTTBroker.h>
#include <SoftwareSerial.h>
/*
 * Setup the credentials for the wifi access point
 */
char ssid[] = "ESP_Access_Point_2";
char pass[] = "helloworld";

/*
 * Setup software serial
 */
SoftwareSerial mySerial;
constexpr SoftwareSerialConfig swSerialConfig = SWSERIAL_8N1;
/*
 * Custom broker class with overwritten callback functions
 */
class myMQTTBroker: public uMQTTBroker
{
public:
    virtual bool onConnect(IPAddress addr, uint16_t client_count) {
      Serial.println(addr.toString()+" connected");
      return true;
    }
    
    virtual bool onAuth(String username, String password) {
      Serial.println("Username/Password: "+username+"/"+password);
      return true;
    }
    
    virtual void onData(String topic, const char *data, uint32_t length) {
      /*
       * Whenever data is received by this broker we send the data over software serial
       */
      char data_str[length+1];
      os_memcpy(data_str, data, length);
      data_str[length] = '\n';
      Serial.println((String)data_str);
      Serial.println("Forwarding ACK");
      mySerial.println((String)data_str);
    }
};

myMQTTBroker myBroker;

/*
 * WiFi init stuff
 */
void startWiFiAP()
{
  /*
   * Code to start the access point
   */
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pass);
  Serial.println("AP started");
  Serial.println("IP address: " + WiFi.softAPIP().toString());
}

void setup()
{
  /*
   * Setup serial and start the access point
   */
  Serial.begin(9600);
  mySerial.begin(9600, swSerialConfig, D4, D3); // RX, TX
  Serial.println();
  Serial.println();
  delay(1000);
  startWiFiAP();

  // Start the broker
  Serial.println("Starting MQTT broker");
  myBroker.init();

/*
 * Subscribe to ack messages
 */
  myBroker.subscribe("ack_messages");
}

void loop()
{
  /*
   * Listen for any incoming messages on the software serial port
   */
  if (mySerial.available()) {
    String message = mySerial.readStringUntil('\n');
    Serial.println("" + message);
    myBroker.publish("esp_messages", "" + message);
  }
}
