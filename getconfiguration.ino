/*
 * EBYTE LoRa E32
 * send a transparent message, you must check that the transmitter and receiver have the same
 * CHANNEL ADDL and ADDH
 *
 *
 * https://www.mischianti.org
 *
 * E32        ----- esp32
 * M0         ----- 19 (or GND)
 * M1         ----- 21 (or GND)
 * RX         ----- TX2 (PullUP)
 * TX         ----- RX2 (PullUP)
 * AUX        ----- 15  (PullUP)
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 *
 */

#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "Enterthehero"; // Enter your WiFi name
const char *password = "Bacovaca";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.mqtt-dashboard.com";
const char *topic = "inTopic1234";
const char *mqtt_username = "esp32lora";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);



#include "Arduino.h"
#include "LoRa_E32.h"
 
// ---------- esp8266 pins --------------
//LoRa_E32 e32ttl(RX, TX, AUX, M0, M1);  // Arduino RX <-- e22 TX, Arduino TX --> e22 RX
//LoRa_E32 e32ttl(D3, D4, D5, D7, D6); // Arduino RX <-- e22 TX, Arduino TX --> e22 RX AUX M0 M1
//LoRa_E32 e32ttl(D2, D3); // Config without connect AUX and M0 M1
 
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(D2, D3); // Arduino RX <-- e22 TX, Arduino TX --> e22 RX
//LoRa_E32 e32ttl(&mySerial, D5, D7, D6); // AUX M0 M1
// -------------------------------------
 
// ---------- Arduino pins --------------
//LoRa_E32 e32ttl(4, 5, 3, 7, 6); // Arduino RX <-- e22 TX, Arduino TX --> e22 RX AUX M0 M1
//LoRa_E32 e32ttl(4, 5); // Config without connect AUX and M0 M1
 
//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(4, 5); // Arduino RX <-- e22 TX, Arduino TX --> e22 RX
//LoRa_E32 e32ttl(&mySerial, 3, 7, 6); // AUX M0 M1
// -------------------------------------
 
// ------------- Arduino MKR WiFi 1010 -------------
// LoRa_E32 e220ttl(&Serial1, 1, 2, 3); //  RX AUX M0 M1
// -------------------------------------------------
 
// ---------- esp32 pins --------------
LoRa_E32 e32ttl(&Serial2, 15, 21, 19); //  RX AUX M0 M1
 
//LoRa_E32 e32ttl(&Serial2, 22, 4, 18, 21, 19, UART_BPS_RATE_9600); //  esp32 RX <-- e22 TX, esp32 TX --> e22 RX AUX M0 M1
// -------------------------------------
 
void setup() {
  Serial.begin(9600);
  delay(500);
 
  // Startup all pins and UART
  e32ttl.begin();
 
  Serial.println("modulo bluetooh lora");
 
  // Send message
  ResponseStatus rs = e32ttl.sendMessage("Hello, world?");
  // Check If there is some problem of succesfully send
  Serial.println(rs.getResponseDescription());

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.println("Connecting to WiFi..");
 }

 Serial.println("Connected to the WiFi network");
 //connecting to a mqtt broker
 client.setServer(mqtt_broker, mqtt_port);
 client.setCallback(callback);
 while (!client.connected()) {
     String client_id = "esp32-client-";
     client_id += String(WiFi.macAddress());
     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
         Serial.println("Public emqx mqtt broker connected");
     } else {
         Serial.print("failed with state ");
         Serial.print(client.state());
         delay(2000);
     }
 }
  client.publish("inTopic1234", "MODULO LORA WIFI READY");
  client.subscribe("inTopic1234");
  
}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");  
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 payload[length] = '\0';
 e32ttl.sendMessage((char *)payload);

 Serial.println();
 Serial.println("-----------------------");
}

 
void loop() {

  client.loop();
    // If something available
  if (e32ttl.available()>1) {
      // read the String message
    ResponseContainer rc = e32ttl.receiveMessage();
 
    // Is something goes wrong print error
    if (rc.status.code!=1){
        Serial.println(rc.status.getResponseDescription());
    }else{
        // Print the data received
        Serial.println(rc.status.getResponseDescription());
        Serial.println(rc.data);
        client.subscribe("inTopic1234");
        client.publish("inTopic1234",String(rc.data).c_str() );
    }
  }
  if (Serial.available()) {
      String input = Serial.readString();
      e32ttl.sendMessage(input);
  }
}
