
#include "SSD1306.h"
#include <Wire.h>
#include<WiFi.h>
#include <PubSubClient.h>

SSD1306 display(0x3c, 21, 22); 

#define ADC_ref 5 // ADC reference Voltage
#define zero_x 0
#define zero_y 0
#define zero_z 0
#define sensitivity_x 0.4
#define sensitivity_y 0.4
#define sensitivity_z 0.4
#define Rod_l 8
#define Tank_l 15
#define Lateral_l 3

const char* ssid = "Nokia 6.1 Plus"; //system wifi
const char* password = "Srihanuman9";
 
#define ORG "jov9sv"
#define DEVICE_TYPE "Ultrasonic"
#define DEVICE_ID "1111"
#define TOKEN "oyN&dT?wMG-cQjy4md"

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/evt/Data/fmt/json"; //for sending we use event (evt), fmt is format
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883,wifiClient);


unsigned int value_x1;
unsigned int value_y1;
unsigned int value_z1;

unsigned int value_x2;
unsigned int value_y2;
unsigned int value_z2;

float xv1;
float yv1;
float zv1;
float angle1;

float xv2;
float yv2;
float zv2;
float angle2;

float Fuel_l;
float Actual_l;
float Est_l;
float vol;
float Err_l;

void setup()
{
Serial.begin(115200);
display.init();
Serial.print("Connecting to ");
 Serial.print(ssid);
 WiFi.begin(ssid, password); //to activate wifi
 while (WiFi.status() != WL_CONNECTED)
 { //to check if connection failed or not
 delay(500);
 Serial.print(".");
 } 
 Serial.println("");
 
 Serial.print("WiFi connected, IP address: ");
 Serial.println(WiFi.localIP());//prints wifi ip address
}


void loop()
{
  display.init();
value_x1 = analogRead(34);
value_y1 = analogRead(35);
value_z1 = analogRead(32);
xv1=(value_x1/1024.0*ADC_ref-zero_x)/sensitivity_x;
yv1=(value_y1/1024.0*ADC_ref-zero_y)/sensitivity_y;
zv1=(value_z1/1024.0*ADC_ref-zero_z)/sensitivity_z;
angle1 =atan2(-yv1,-zv1)*57.2957795+180;
Serial.println(xv1);
Serial.println(yv1);
Serial.println(zv1);
Serial.println(angle1);
Fuel_l=Rod_l*cos(angle1);
Est_l=Tank_l-Fuel_l;
value_x2 = analogRead(36);
value_y2 = analogRead(37);
value_z2 = analogRead(38);
xv2=(value_x2/1024.0*ADC_ref-zero_x)/sensitivity_x;
yv2=(value_y2/1024.0*ADC_ref-zero_y)/sensitivity_y;
zv2=(value_z2/1024.0*ADC_ref-zero_z)/sensitivity_z;
angle2 =atan2(-yv2,-zv2)*57.2957795+180;
//delay(1000);
Serial.println(xv2);
Serial.println(yv2);
Serial.println(zv2);
Serial.println(angle2);
Err_l=Lateral_l*tan(angle2);
Actual_l=Est_l-Err_l;
vol=(Actual_l)*(Tank_l)*(Tank_l);
Serial.println("Actual vol="+String(vol));
//delay(500);
display.drawString(0, 0, "Hello");
 String ch=String(vol);
display.drawString(0,40,"FinalVol="+ch);
display.display();
delay(3000);
//display.setTextSize(1);
//display.setTextColor(WHITE);
//display.setCursor(0,10);
//display.drawString(0,20,"FinalVol=");
//display.display();
//
//display.setCursor(0,20);
//display.print("Vtilt="+String(angle2));
//display.display();
//
//display.setCursor(0,30);
//display.print("Rtilt="+String(angle1));
//display.display();

PublishData(angle1,angle2,vol);

}


void PublishData(float angle1, float angle2, float vol)
{
 if (!!!client.connected()) //to check connection
 {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token))
 {
   Serial.print(".");
   delay(500);
 }
 Serial.println();
 }
  String payload = "{\"d\":{\"Rod_angle\":";
  payload += angle1;
  payload+="," "\"Vehice_angle\":";
  payload += angle2;
  payload+="," "\"Fuel_vol\":";
  payload += vol;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(topic, (char*) payload.c_str())) //to check if data is published
 { 
 Serial.println("Publish ok");
 } 
 else 
 {
 Serial.println("Publish failed");
 }
}
