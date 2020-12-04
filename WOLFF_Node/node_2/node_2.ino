#include <TheThingsNetwork.h>
#include <SoftwareSerial.h>
#define FCNTBUFFERSIZE  12

// Set your DevAddr, NwkSKey, AppSKey and the frequency plan
const char *devAddr = "12345678";
const char *nwkSKey = "ffffffffffffffffffffffffffffffff";
const char *appSKey = "ffffffffffffffffffffffffffffffff";

#define loraSerial Serial1
#define debugSerial Serial
SoftwareSerial mySerial(11, 12); // RX, TX

// Replace REPLACE_ME with TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_US915

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);

void setup()
{
  /*
   * Setup the frame counter buffer and setup serial
   */
  char fCntBuffer[FCNTBUFFERSIZE];
  int fCntBufPtr = 0;
  loraSerial.begin(57600);
  debugSerial.begin(9600);

  // Wait a maximum of 10s for Serial Monitor
  while (!debugSerial && millis() < 10000)
    ;
  debugSerial.println("Serial ready");
  mySerial.begin(9600);
  while(loraSerial.available()) {
      debugSerial.write(loraSerial.read());   // Clear the LoRa module's serial output
    }
  /*
   * Get the frame counter and read it into the frame counter buffer
   */
  debugSerial.println("-- GET UPCTR");
  loraSerial.print("mac get upctr\r\n");    // write the MAC command to the LoRa module to retrieve Up fCnt
  delay(100);                               // may not be necessary; give the module time to respond
  while(loraSerial.available() && (fCntBufPtr < FCNTBUFFERSIZE) ) {
    fCntBuffer[fCntBufPtr++] = loraSerial.read();   // read and store the next character of the Up fCnt
  }
  fCntBuffer[fCntBufPtr++] = '\0';            // add null termination to the string
  debugSerial.write("Stored up counter: ");   // show the user what the stored Up fCnt was
  debugSerial.write(fCntBuffer);              //
  debugSerial.println("-- END GET UPCTR");
  
  //Register the function to receive downlink messages
  ttn.onMessage(message);

  /*
   * Start the LoRa module in ABP mode
   */
  debugSerial.println("-- PERSONALIZE");
  ttn.personalize(devAddr, nwkSKey, appSKey);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- SET UPCTR");
  loraSerial.print((String)"mac set upctr " + fCntBuffer + "\r\n"); // send the MAC command to restore the saved Up fCnt
  delay(100);
  while(loraSerial.available()) {
    debugSerial.write(loraSerial.read());     // LoRa module should respond with "ok"
  }
  debugSerial.println("-- END SET UPCTR");
 
  debugSerial.println("-- GET UPCTR");        // THIS ENTIRE BLOCK IS OPTIONAL: 
  loraSerial.print("mac get upctr\r\n");      // here we simply show that the working copy of the
  delay(100);                                 // Up fCnt has been set to the previously stored value.
  while(loraSerial.available()) {
    debugSerial.write(loraSerial.read());     // print the new value
  }
  debugSerial.println("-- END GET UPCTR");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  const int saveInterval = 5;         // save the Up fCnt every "this many" messages sent
  static int sendCnt = saveInterval;  // establish a countdown for saving Up fCnt

  /*
   * if incoming message is ready go into the if statement
   */
  if (mySerial.available()) {
    /*
     * Save the frame counter for every saveInterval number of messages
     */
    if(sendCnt == 0) {
      debugSerial.println("Sending MAC SAVE");  //
      loraSerial.print( "mac save\r\n");        // Send the "mac save" command to the LoRa module
      delay(1000);                              // Give it time to write to flash (unsure how long needed)
      sendCnt = saveInterval;                   // Reset countdown for saving Up fCn
      while(loraSerial.available()) {           // Wait for "ok" response
        debugSerial.write(loraSerial.read());   // Print the response to debug port
      }
    }

    /*
     * Get the frame counter
     */
    loraSerial.print("mac get upctr\r\n");
    delay(100);
    debugSerial.print("upctr = ");
    while(loraSerial.available()) {
      debugSerial.write(loraSerial.read());
    }

    /*
     * Send the incoming message over LoRa
     */
    String message = mySerial.readStringUntil('\n');
    Serial.println("");
    Serial.println("You sent: " + message);
    Serial.println("sending: " + message);
    byte payload[message.length()];
    message.getBytes(payload, message.length());
    ttn.sendBytes(payload, sizeof(payload));
    sendCnt--;
  }

  /*
     * Save the frame counter for every saveInterval number of messages
     */
  if(sendCnt == 0) {
      debugSerial.println("Sending MAC SAVE");  //
      loraSerial.print( "mac save\r\n");        // Send the "mac save" command to the LoRa module
      delay(1000);                              // Give it time to write to flash (unsure how long needed)
      sendCnt = saveInterval;                   // Reset countdown for saving Up fCn
      while(loraSerial.available()) {           // Wait for "ok" response
        debugSerial.write(loraSerial.read());   // Print the response to debug port
      }
    }

  loraSerial.print("mac get upctr\r\n");
  delay(100);
  debugSerial.print("upctr = ");
  while(loraSerial.available()) {
    debugSerial.write(loraSerial.read());
  }

  /*
   * Poll the gateway with 1 byte
   */
  debugSerial.println("-- LOOP");
  byte payload[1];
  payload[0] = 10;
  ttn.sendBytes(payload, sizeof(payload));
  sendCnt--;
}

/*
 * This is the callback for any incoming LoRa messages
 */
void message(const byte* payload, size_t length, port_t port) {
  /*
   * Send the message to the ESP module via serial
   */
  debugSerial.println("-- MESSAGE");
  debugSerial.print("Received " + String(length) + " bytes on port " + String(port) + ":");
  String message = String((char *) payload);
  mySerial.println(message);
  debugSerial.write(payload, length);
  debugSerial.println();
}
