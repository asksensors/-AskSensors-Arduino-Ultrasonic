/*
 * Description: Distance measurement using ultrasonic sensor with Arduino connected to AskSensors IoT Platform (https://asksensors.com) through an ESP8266 WiFi module.
 *  Author: https://asksensors.com, 2018
 *  github: https://github.com/asksensors
 */
 

#include <SoftwareSerial.h>

// serial config
#define     RX    10
#define     TX    11

// utlrasonic pinout
#define ULTRASONIC_TRIG_PIN     9   // pin TRIG 
#define ULTRASONIC_ECHO_PIN     8   // pin ECHO 


SoftwareSerial AT(RX,TX); 

// TODO: change user config
String ssid     = "............."; //Wifi SSID
String password = "............."; //Wifi Password
String apiKeyIn = "............."; // API Key
const unsigned int writeInterval = 25000; // write interval (in ms)

// ASKSENSORS API host config
String host = "api.asksensors.com";  // API host name
String port = "80";      // port

int AT_cmd_time; 
boolean AT_cmd_result = false; 

void setup() {
  Serial.begin(9600);
  // open serial 
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : Connect Ultrasonic HC-SR04 + Arduino WiFi to AskSensors");
  AT.begin(115200);
  Serial.println("Initiate AT commands with ESP8266 ");
  sendATcmd("AT",5,"OK");
  sendATcmd("AT+CWMODE=1",5,"OK");
  Serial.print("Connecting to WiFi:");
  Serial.println(ssid);
  sendATcmd("AT+CWJAP=\""+ ssid +"\",\""+ password +"\"",20,"OK");

    // ultraonic setup 
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
}

void loop() {
  // measure distance
  long duration, distance;
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH);
  distance = (duration/2) / 29.1;
  Serial.print("********** Ultrasonic Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

 // Create the URL for the request
  String url = "GET /write/";
  url += apiKeyIn;
  url += "?module1=";
  url += distance;
  
  Serial.println("*****************************************************");
  Serial.println("********** Open TCP connection ");
  sendATcmd("AT+CIPMUX=1", 10, "OK");
  sendATcmd("AT+CIPSTART=0, \"TCP\",\"" + host +"\"," + port, 20, "OK");
  sendATcmd("AT+CIPSEND=0," + String(url.length() + 4), 10, ">");
  
  Serial.print("********** requesting URL: ");
  Serial.println(url);
  AT.println(url);
  delay(2000);
  sendATcmd("AT+CIPCLOSE=0", 10, "OK");
  
  Serial.println("********** Close TCP Connection ");
  Serial.println("*****************************************************");
  
  delay(writeInterval);   // delay
  }



// sendATcmd
void sendATcmd(String AT_cmd, int AT_cmd_maxTime, char readReplay[]) {
  Serial.print("AT command:");
  Serial.println(AT_cmd);

  while(AT_cmd_time < (AT_cmd_maxTime)) {
    AT.println(AT_cmd);
    if(AT.find(readReplay)) {
      AT_cmd_result = true;
      break;
    }
  
    AT_cmd_time++;
  }
  Serial.print("...Result:");
  if(AT_cmd_result == true) {
    Serial.println("DONE");
    AT_cmd_time = 0;
  }
  
  if(AT_cmd_result == false) {
    Serial.println("FAILED");
    AT_cmd_time = 0;
  }
  
  AT_cmd_result = false;
}
