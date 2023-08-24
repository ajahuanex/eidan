#include <ESP8266WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);
//Declarcion de variables
 const char* mqtt_server = "eidan.paputec.com";
 const uint16_t mqtt_port = 8883; 
 const char* mqttTopicIn = "mqttIn";
 const char* mqttTopicOut = "mqttOut";

 int p1 = 13;
 int p2 = 12;
 int pL = 14;
 int pR = 4;
// Escucha las ordenes desde el servidor
void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // message received

  Serial.print("Message arrived on topic: '");
   Serial.print(topic);
  Serial.print("' with payload: ");
  //Serial.print(length +'length');
   for (unsigned int i = 0; i < length; i++) {
     //Serial.print((char)payload[i]);
     
   }
  
    switch((char)payload[0]) {
    case '1':
      //do something when var equals 1
      Serial.println('1');
      digitalWrite(p1,LOW);
      break;
      case '2':
      //do something when var equals 1
      Serial.println('2');
      digitalWrite(p1,HIGH);
      break;
    case '3':
      //do something when var equals 2
      Serial.println('3');
      digitalWrite(p2,LOW);
      break;
    case '4':
      //do something when var equals 1
      Serial.println('4');
      digitalWrite(p2,HIGH);
      break;
    case 'L':
      Serial.println('L');
      digitalWrite(pL,LOW);
      break;
    case 'M':
      Serial.println('M');
      digitalWrite(pL,HIGH);
      break;
    case 'R':
      Serial.println('R');
      digitalWrite(pR,LOW);
      break;
    case 'S':
      Serial.println('S');
      digitalWrite(pR,HIGH);
      break;  
    default:
      // if nothing else matches, do the default
      // default is optional
      Serial.print('A');
      break;
  }
  Serial.println();
  
 String myCurrentTime = String(random(300));
  client.publish(mqttTopicOut,("Time: " + myCurrentTime).c_str());
  // Switch on the LED if an 1 was received as first character

}
//Busca reconectarse al servidor por el protocolo mqtt
void mqttReconnect() {
    // reconnect code from PubSubClient example
    while (!client.connected()) {
     Serial.print("Attempting MQTT connection...");
     String mqttClientId = "";
     if (client.connect(mqttClientId.c_str())){
       Serial.println("connected");
       client.subscribe(mqttTopicIn);
     } else {
       Serial.print("failed, rc=");
       Serial.print(client.state());
       Serial.println(" will try again in 5 seconds");
       delay(5000);
     }
   }
}
//configuraciones
void setup() {
    WiFi.mode(WIFI_STA); 
    // put your setup code here, to run once:
    Serial.begin(9600);
// pines del esp como salidas al control remoto
  pinMode(p1,OUTPUT);
  pinMode(p2,OUTPUT);
  pinMode(pL,OUTPUT);
  pinMode(pR,OUTPUT);
    

    WiFiManager wm;

    wm.resetSettings();

    bool res;

    res = wm.autoConnect("AutoConnectAP","password"); 

    if(!res) {
        Serial.println("Failed to connect");
        
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");

        client.setServer(mqtt_server, mqtt_port);
        client.setCallback(mqttCallback);
    }

}
//Bucles
void loop() {
    // put your main code here, to run repeatedly:  
       if (!client.connected()) {
    mqttReconnect();
  }
  client.loop();
  yield(); 
}
