#include <Wire.h>
#include <CayenneMQTTESP32.h>
#include "max32664.h"
#define RESET_PIN 04
#define MFIO_PIN 02
#define RAWDATA_BUFFLEN 250
#define CAYENNE_PRINT Serial     // Comment this out to disable prints and save space
char username[] = "eb848d90-3602-11ec-9f5b-45181495093e";
char password[] = "f492a57c1a5cb6cbb0e4bfc81dbf8ceedd9947d4";
char clientID[] = "714dba40-7703-11ed-8d53-d7cd1025126a";
char ssid[]     = "Redmi K50";
char wifiPassword[] = "qwerty1234";

max32664 MAX32664(RESET_PIN, MFIO_PIN, RAWDATA_BUFFLEN);
void mfioInterruptHndlr(){
  //Serial.println("i");
}

void enableInterruptPin(){
attachInterrupt(digitalPinToInterrupt(MAX32664.mfioPin), mfioInterruptHndlr, FALLING);
}

void loadAlgomodeParameters(){

  algomodeInitialiser algoParameters;

  algoParameters.calibValSys[0] = 120;
  algoParameters.calibValSys[1] = 122;
  algoParameters.calibValSys[2] = 125;

  algoParameters.calibValDia[0] = 80;
  algoParameters.calibValDia[1] = 81;
  algoParameters.calibValDia[2] = 82;

 
  MAX32664.loadAlgorithmParameters(&algoParameters);
}



void setup(){
  pinMode(5,OUTPUT);
  Serial.begin(115200);
  Cayenne.begin(username, password, clientID, ssid, wifiPassword);

  Wire.begin();

  loadAlgomodeParameters();

  int result = MAX32664.hubBegin();
  if (result == CMD_SUCCESS){
    Serial.println("Sensorhub begin!");
    }
    else{
    //stay here.
    while(1){
      Serial.println("Could not communicate with the sensor! please make proper connections");
      delay(5000);
    }
  }

  bool ret = MAX32664.startBPTcalibration();
  while(!ret){

    delay(10000);
    Serial.println("failed calib, please retsart");
  }

  delay(1000);

  ret = MAX32664.configAlgoInEstimationMode();
  while(!ret){

    ret = MAX32664.configAlgoInEstimationMode();
    delay(10000);
  }
  for(int i=0;i<3;i++){
  digitalWrite(5,HIGH);
  delay(200);
  digitalWrite(5,LOW);
  delay(200);
  }
  Serial.println("Getting the device ready..");
  delay(1000);
}

void loop(){
  Cayenne.loop();
  uint8_t num_samples = MAX32664.readSamples();

  if(num_samples){
    int sys =MAX32664.max32664Output.sys;
    int dia =MAX32664.max32664Output.dia;
    int hr  =MAX32664.max32664Output.hr;
    Serial.print("sys = ");
    Serial.print(sys);
    Serial.print(", dia = ");
    Serial.print(dia);
    Serial.print(", hr = ");
    Serial.println(hr);
    Cayenne.virtualWrite(1,sys);
    Cayenne.virtualWrite(2,dia);
    Cayenne.virtualWrite(3,hr);
 
}
  delay(100);
  int hr  =MAX32664.max32664Output.hr;
  if(hr>120){
  for(int i=0;i<10;i++){
  digitalWrite(5,HIGH);
  delay(200);
  digitalWrite(5,LOW);
  delay(200);
  }
   }
}
