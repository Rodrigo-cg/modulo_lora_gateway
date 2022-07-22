#include <WiFi.h>
#include <PubSubClient.h>
///
#include <HardwareSerial.h>
#define rx 16                                          //LORA TX
#define tx 17                                          //LORA RX  

HardwareSerial myserial(1);

String datastring="";
bool dataComplete=false;
///
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

void setup() {
 // Set software serial baud to 115200;
 Serial.begin(115200);
 // connecting to a WiFi network
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
 // publish and subscribe
 client.publish("inTopic1234", "Hi EMQX I'm ESP32 ^^");
 client.subscribe("inTopic1234");
 myserial.begin(9600, SERIAL_8N1, rx, tx);
}

void callback(char *topic, byte *payload, unsigned int length) {
 Serial.print("Message arrived in topic: ");  
 Serial.println(topic);
 Serial.print("Message:");
 for (int i = 0; i < length; i++) {
     Serial.print((char) payload[i]);
 }
 Serial.println();
 Serial.println("-----------------------");
}
void serialEvent(){
  while(myserial.available()){
    char inChar=(char)myserial.read();
    datastring+=inChar;
    if(inChar =='\n'){
      dataComplete=true;
    }
  }
}
void loop() {
 client.loop();
   if (myserial.available()) serialEvent();
   if(dataComplete){
      Serial.println(String(datastring));

      client.subscribe("inTopic1234");
      client.publish("inTopic1234", String(datastring).c_str());
      
  }

  delay(5000);
}
