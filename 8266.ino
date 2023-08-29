/**
 * WiFiManager advanced demo, contains advanced configurartion options
 * Implements TRIGGEN_PIN button press, press for ondemand configportal, hold for 3 seconds for reset settings.
 */
#include <ESP8266WiFi.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <PubSubClient.h>

#define TRIGGER_PIN 0

// wifimanager can run in a blocking mode or a non blocking mode
// Be sure to know how to process loops with no delay() if using non blocking
bool wm_nonblocking = false; // change to true to use non blocking

WiFiManager wm; // global wm instance
WiFiManagerParameter custom_field; // global param ( for non blocking w params )

WiFiClient espClient;
PubSubClient client(espClient);

 const char* mqtt_server = "eidan.paputec.com";
 const uint16_t mqtt_port = 8883; 
 const char* mqttUser = "";
 const char* mqttPassword = "";
 const char* mqttTopicIn = "mqttIn";
 const char* mqttTopicOut = "mqttOut";

 const char* ssid = "OnDemandAP";
 const char* passwd = "password";

 int p1 = 14;
 int p2 = 12;
 int pL = 13;
 int pR = 15;

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
      digitalWrite(p1,HIGH);
      
      break;
    case '2':
      //do something when var equals 2
      Serial.println('2');
      digitalWrite(p2,HIGH);
      break;
    case 'L':
      Serial.println('L');
      digitalWrite(pL,HIGH);
      break;
    case 'R':
      Serial.println('R');
      digitalWrite(pR,HIGH);
      break;
    default:
      // if nothing else matches, do the default
      // default is optional
      Serial.print('de');
      break;
  }
  Serial.println();
  
 String myCurrentTime = String(random(300));
  client.publish(mqttTopicOut,("Time: " + myCurrentTime).c_str());
  // Switch on the LED if an 1 was received as first character





    
}

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


void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP  
  Serial.begin(9600);
  Serial.setDebugOutput(true);  
  delay(3000);
  Serial.println("\n Starting");

  pinMode(TRIGGER_PIN, INPUT);

  pinMode(p1,OUTPUT);
  pinMode(p2,OUTPUT);
  pinMode(pL,OUTPUT);
  pinMode(pR,OUTPUT);
  
  // wm.resetSettings(); // wipe settings

  if(wm_nonblocking) wm.setConfigPortalBlocking(false);

  // add a custom input field
  int customFieldLength = 40;


  // new (&custom_field) WiFiManagerParameter("customfieldid", "Custom Field Label", "Custom Field Value", customFieldLength,"placeholder=\"Custom Field Placeholder\"");
  
  // test custom html input type(checkbox)
  // new (&custom_field) WiFiManagerParameter("customfieldid", "Custom Field Label", "Custom Field Value", customFieldLength,"placeholder=\"Custom Field Placeholder\" type=\"checkbox\""); // custom html type
  
  // test custom html(radio)
  const char* custom_radio_str = "<br/><label for='customfieldid'>Custom Field Label</label><input type='radio' name='customfieldid' value='1' checked> One<br><input type='radio' name='customfieldid' value='2'> Two<br><input type='radio' name='customfieldid' value='3'> Three";
  new (&custom_field) WiFiManagerParameter(custom_radio_str); // custom html input
  
  wm.addParameter(&custom_field);
  wm.setSaveParamsCallback(saveParamCallback);
  std::vector<const char *> menu = {"wifi","info","param","sep","restart","exit"};
  wm.setMenu(menu);
  wm.setClass("invert");
  wm.setConfigPortalTimeout(80); // auto close configportal after n seconds
  bool res;
  res = wm.autoConnect("AutoConnectAP",passwd); // password protected ap

  if(!res) {
    Serial.println("Failed to connect or hit timeout");
    // ESP.restart();
  } 
  else {
    //if you get here you have connected to the WiFi    
    Serial.println("connected...yeey :)");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
  }

}

void checkButton(){
  // check for button press
  if ( digitalRead(TRIGGER_PIN) == LOW ) {
    // poor mans debounce/press-hold, code not ideal for production
    delay(50);
    if( digitalRead(TRIGGER_PIN) == LOW ){
      Serial.println("Button Pressed");
      // still holding button for 3000 ms, reset settings, code not ideaa for production
      delay(3000); // reset delay hold
      if( digitalRead(TRIGGER_PIN) == LOW ){
        Serial.println("Button Held");
        Serial.println("Erasing Config, restarting");
        wm.resetSettings();
        ESP.restart();
      }
      
      // start portal w delay
      Serial.println("Starting config portal");
      wm.setConfigPortalTimeout(240);
      
      if (!wm.startConfigPortal(ssid,passwd)) {
        Serial.println("failed to connect or hit timeout");
        delay(3000);
        // ESP.restart();
      } else {
        //if you get here you have connected to the WiFi
        Serial.println("connected...yeey :)");
 client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);        
      }
    }
  }
}


String getParam(String name){
  //read parameter from server, for customhmtl input
  String value;
  if(wm.server->hasArg(name)) {
    value = wm.server->arg(name);
  }
  return value;
}

void saveParamCallback(){
  Serial.println("[CALLBACK] saveParamCallback fired");
  Serial.println("PARAM customfieldid = " + getParam("customfieldid"));
}

void loop() {
  if(wm_nonblocking) wm.process(); // avoid delays() in loop when non-blocking and other long running code  
  checkButton();
  // put your main code here, to run repeatedly:
   if (!client.connected()) {
    mqttReconnect();
  }
  client.loop();
  yield();
}
