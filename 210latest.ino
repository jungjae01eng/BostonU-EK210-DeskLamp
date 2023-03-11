#include <SoftwareSerial.h>
#include "VoiceRecognitionV3.h"
#include <Arduino.h>
#include <hp_BH1750.h>  //  include the library
hp_BH1750 BH1750;       //  create the sensor
int BLED=9;
int RLED=10;
int GLED=11;
VR myVR(2,3);
uint8_t records[7]; // save record
uint8_t buf[64];

int led = 13;
int cmd=23;

#define onRecord    (0)
#define offRecord   (1) 

void printSignature(uint8_t *buf, int len)
{
  int i;
  for(i=0; i<len; i++){
    if(buf[i]>0x19 && buf[i]<0x7F){
      Serial.write(buf[i]);
    }
    else{
      Serial.print("[");
      Serial.print(buf[i], HEX);
      Serial.print("]");
    }
  }
}

void printVR(uint8_t *buf)
{
  Serial.println("VR Index\tGroup\tRecordNum\tSignature");

  Serial.print(buf[2], DEC);
  Serial.print("\t\t");

  if(buf[0] == 0xFF){
    Serial.print("NONE");
  }
  else if(buf[0]&0x80){
    Serial.print("UG ");
    Serial.print(buf[0]&(~0x80), DEC);
  }
  else{
    Serial.print("SG ");
    Serial.print(buf[0], DEC);
  }
  Serial.print("\t");

  Serial.print(buf[1], DEC);
  Serial.print("\t\t");
  if(buf[3]>0){
    printSignature(buf+4, buf[3]);
  }
  else{
    Serial.print("NONE");
  }
  Serial.println("\r\n");
}

void setup() {
  // put your setup code here, to run once:
 myVR.begin(9600);
  
  Serial.begin(115200);
  Serial.println("Elechouse Voice Recognition V3 Module\r\nControl LED sample");
  
  pinMode(led, OUTPUT);
    
  if(myVR.clear() == 0){
    Serial.println("Recognizer cleared.");
  }else{
    Serial.println("Not find VoiceRecognitionModule.");
    Serial.println("Please check connection and restart Arduino.");
    while(1);
  }
  
  if(myVR.load((uint8_t)onRecord) >= 0){
    Serial.println("onRecord loaded");
  }
  
  if(myVR.load((uint8_t)offRecord) >= 0){
    Serial.println("offRecord loaded");
  }
  Serial.begin(9600);
  bool avail = BH1750.begin(BH1750_TO_GROUND);
  
  if (!avail) {
    Serial.println("No BH1750 sensor found!");
    while (true) {};                                        
  }
}


void loop() 
{
 int ret;
  BH1750.start();   //starts a measurement
  float lux=BH1750.getLux();  //  waits until a conversion finished
  Serial.begin(115200);
  Serial.println("Current Lux is: ");
  Serial.println(lux);   
  delay(100); 
  if (lux > 300||cmd==0)
  {
  analogWrite(RLED,0);
  analogWrite(GLED,0);
  analogWrite(BLED,0);
  }
  else if((lux<300)||(cmd==1))
  {
    //Serial.println("case two true");
  analogWrite(RLED,255);
  analogWrite(GLED,255);
  analogWrite(BLED,255);
  }
  ret = myVR.recognize(buf, 50);
  Serial.begin(115200);
  if(ret>0){
    switch(buf[1]){
      case onRecord:
      {
        digitalWrite(led, HIGH);
        cmd=1;
        Serial.println("command is:");
        Serial.println(cmd);
        Serial.println("CASE 1");
        break;
      }
   
      case offRecord:
      {
        cmd=0;
        digitalWrite(led, LOW);
        Serial.println("CASE 2");
        Serial.println(cmd);
        break;
      }
      default:
        Serial.println("Record function undefined");
        break;
    }
    /** voice recognized */
    printVR(buf);
  }
}
