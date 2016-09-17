/*
** Markus van Kempen - mvk@ca.ibm.com - 20160914
*/
// This #include statement was automatically added by the Particle IDE.
#include "InternetButton/InternetButton.h"

// This #include statement was automatically added by the Particle IDE.
#include "MQTT/MQTT.h"

InternetButton b = InternetButton();
/*
// IOT INFOS
*/
char *IOT_CLIENT= "d:YOUR:photon:blinker"; // ###UPDATE
char *IOT_HOST     = "YOUR.messaging.internetofthings.ibmcloud.com"; //##UPDATE
char *IOT_PASSWORD = "YOURTOKEN"; //##UPDATE
char *IOT_PUBLISH  = "iot-2/evt/count/fmt/json"; 
char *IOT_PUBLISH_CMD = "iot-2/evt/status/fmt/json"; 
char *IOT_PUBLISH_ALIVE = "iot-2/evt/alive/fmt/json"; 
char *IOT_USERNAME  = "use-token-auth";
char *IOT_SUBSCRIBE = "iot-2/cmd/+/fmt/json";
unsigned long lastTime = 0UL;
int blinker_left=0;
int blinker_right=0;

MQTT client( IOT_HOST, 1883, callback );

void setup() { 
 b.begin();
 b.setBrightness(90);
  b.allLedsOff();
  b.ledOn(11, 0, 255, 0);
 
 Serial.begin(9600);
  Serial.println("Hello -ver20160829");
 
 client.connect( 
 IOT_CLIENT, 
 IOT_USERNAME, 
 IOT_PASSWORD 
 );

 if( client.isConnected() ) {
        Serial.print("IOTF is connected");
         Serial.print("Subscribed to all cmd");
     client.publish( 
 IOT_PUBLISH, 
 "{\"msg\": 'startup'}" 
 );
 client.subscribe( IOT_SUBSCRIBE );
 b.ledOn(11, 0, 255, 0);
 } else {
 b.ledOn(11,255,0,0);
 }
 
}

//Main processing loop, just look for buttons and keep the MQTT pipe alive
int debounce = 0; 
int mynum = 1; 

void loop() { 

 if (!client.isConnected()) { 
  Serial.println("Reconnecting client to ");
  Serial.println(IOT_HOST);
 while ( client.connect(IOT_CLIENT,IOT_USERNAME, IOT_PASSWORD)) 
 {
 Serial.print(".");
 delay(2000);
 if (client.isConnected())
  client.subscribe( IOT_SUBSCRIBE );
 }
 Serial.println();
      
 }else{
   
    // Serial.println("Connected to MQTT ");
     //client.subscribe("iot-2/cmd/+/fmt/json");
}

//blinker

    if (blinker_right == 1 )
    {
          b.ledOn(2,255,0,0);
          b.ledOn(3,255,0,0);
          b.ledOn(4,255,0,0);
    }else{
         b.ledOff(2);
         b.ledOff(3);
         b.ledOff(4);
    }
    
    if (blinker_left == 1 )
    {
         b.ledOn(8,255,0,0);
        b.ledOn(9,2555,0,0);
        b.ledOn(10,255,0,0);
    }else{
         b.ledOff(8);
         b.ledOff(9);
         b.ledOff(10);
    }
 
 // B2
 if (b.buttonOn(2)) {

 b.ledOn(3,255,255,255);
 //debounce = 0;
 if(blinker_right == 1)
 {
    Serial.println("Button 2 pressed - blinker_right off");
    client.publish( IOT_PUBLISH_CMD, "{\"d\":{\"cmd\": \"blinker_right_off\"}}" );
    blinker_right = 0;
 } else {
    Serial.println("Button 2 pressed - blinker_right ON");
    client.publish( IOT_PUBLISH_CMD, "{\"d\":{\"cmd\": \"blinker_right_on\"}}" );
    blinker_right = 1;
    blinker_left = 0;
    }
}//B2
 
 // B4
 if (b.buttonOn(4)) {

 b.ledOn(9,255,255,255);
 //debounce = 0;
 if(blinker_left == 1)
 {
    Serial.println("Button 4 pressed - blinker_left off");
    client.publish( IOT_PUBLISH_CMD, "{\"d\":{\"cmd\": \"blinker_left_off\"}}" );
    blinker_left = 0;
 } else {
    Serial.println("Button 4 pressed - blinker_left ON");
    client.publish( IOT_PUBLISH_CMD, "{\"d\":{\"cmd\": \"blinker_left_on\"}}" );
    blinker_right = 0;
    blinker_left = 1;
    }
}//B4


 
 
 
  unsigned long now = millis();
//send alive message every 5 sec.    
if (now-lastTime>5000UL) {
        lastTime = now;
   Serial.println("Publish Alive message ver4");    
 
 if (client.publish(  IOT_PUBLISH_ALIVE,  "{\"d\":{\"msg\": \"alive\"}}"  )) {
    b.allLedsOn(0,20,20);
    delay(100);
    b.allLedsOff();
 }else{
      Serial.println("Error Publish Alive message ");    
    b.allLedsOn(255,0,0);
    delay(500);
    b.allLedsOff();
 }     
 
}        


 client.loop();


 delay(500);
 b.allLedsOff();
 }// loop

void callback( char* topic, byte* payload, unsigned int length ) { 

    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    String message(p);
    Serial.println("<<<<<< Got a MQTT message");
    Serial.println(message);//{"d":{"action":"ON","appliance":"blinker","direction":"left","colorLeft":"255,0,0","colorRight":"255,0,0","ts":1473745045922}}
    int start=message.indexOf("action\":\"")+9;
    String cmd = message.substring(start, start+2);
    
    int dirstart=message.indexOf("direction\":\"")+12;
    String dircmd = message.substring(dirstart, dirstart+4);
    Serial.println(cmd);   // cmd ON OFF
    
    if ( cmd.equals("OF") )
    {
         Serial.println("Switching Blinker OFF");
            blinker_left=0;
            blinker_right=0;
         b.allLedsOff();
    }
    
    if (cmd.equals("ON") )
    {
        Serial.println("Switching Blinker "+dircmd+" ON ");
        if (dircmd.equals("left") )
        {
             blinker_left=1;
            blinker_right=0;
        }
        if (dircmd.equals("righ") )     
        {
            blinker_right=1;
            blinker_left=0;
        }
    }
    
    
    
 b.ledOn(11, 0, 255, 255);
 
  delay(250);
 b.ledOff(11);
}//callback
