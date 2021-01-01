#include <HTTPClient.h>
#include <WiFi.h> 
#include <SDI12.h>

#define DATA_PIN 13
#define SENSOR_ADDRESS 0

const char* ssid = "MY WIFI 6E8E";           
const char* password = "MYWIFI5304"; 
String url = "https://soilmoisturedatalogger.herokuapp.com/add/data?data=";
HTTPClient http;
String SN = "20201220";
String sdiResponse = "";
String command = "";
float VWCRaw;
float VWC;
float temperature;
float ec;
SDI12 mySDI12(DATA_PIN);

//function protitypes
String takeMeasurements();
void extractDatas(String d);

void setup()
{
 
  Serial.begin(9600);

  WiFi.begin(ssid, password);

//  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.print(".");
  }
  mySDI12.begin();
  delay(500);
//  Serial.println();
//  Serial.print("Connected, IP address: ");
//  Serial.println(WiFi.localIP());
}

void loop() {

    String myData = takeMeasurements();
    delay(1000);
    extractDatas(myData);
    VWC = (3.879 * VWCRaw)/(10000.0) - 0.6956;
//    Serial.println(VWCRaw);
//    Serial.println(VWC);
//    Serial.println(temperature);
//    Serial.println(ec);

  
    // reconnect to the wifi it is accidentally disconnected
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }

    String path = url + SN + "*" + VWC + "*" + temperature + "*" + ec;
   
    http.begin(path.c_str());

    //send get request to the web
    
    int httpResponseCode = http.GET();
    
      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
//        Serial.print("Error code: ");
//        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
      delay(10000);

  
  
  }

  String takeMeasurements(){
      command = String(SENSOR_ADDRESS) + "M!";
      mySDI12.sendCommand(command);
      delay(30);
  //Serial.println(command);
     while(mySDI12.available()){
        char c = mySDI12.read();
      if ((c != '\n') && (c != '\r')) {
         sdiResponse += c;
         delay(5);
        }
     }
//    if (sdiResponse.length() > 1);
    sdiResponse = "";
    mySDI12.clearBuffer();
    delay(1000);
    command = String(SENSOR_ADDRESS) + "D0!";
    mySDI12.sendCommand(command);
    delay(30);
  
  //Serial.println(command);
    while(mySDI12.available()){
        char c = mySDI12.read();
     if ((c != '\n') && (c != '\r')) {
        sdiResponse += c;
        delay(5);
       }
     }
//   if (sdiResponse.length() > 1);
//   //Serial.println(sdiResponse);
    mySDI12.clearBuffer();
    delay(1000);

    command = String(SENSOR_ADDRESS) + "D1!";
    mySDI12.sendCommand(command);
    delay(30);
  
  //Serial.println(command);
    while(mySDI12.available()){
        char c = mySDI12.read();
     if ((c != '\n') && (c != '\r')) {
         sdiResponse += c;
         delay(5);
        }
     }
    //if (sdiResponse.length() > 1);
    //Serial.println(sdiResponse);
    mySDI12.clearBuffer();
    delay(1000);
 
    command = String(SENSOR_ADDRESS) + "D2!";
    mySDI12.sendCommand(command);
    delay(30);
  
    //Serial.println(command);
    while(mySDI12.available()){
         char c = mySDI12.read();
      if ((c != '\n') && (c != '\r')) {
         sdiResponse += c;
         delay(5);
        }
    }
    //if (sdiResponse.length() > 1);
   //Serial.println(sdiResponse);
    mySDI12.clearBuffer();
    return sdiResponse;
 }

 void extractDatas(String d){
    int index1 = d.indexOf('+');
    int index2 = d.indexOf('+',index1 + 1);
    int index3 = d.indexOf('+',index2 + 1);
    int index4 = d.indexOf('+',index3 + 1);
    VWCRaw          =  d.substring(index1 + 1,index2).toFloat();
    temperature  =  d.substring(index2 + 1,index3).toFloat();
    ec           =  d.substring(index3 + 1,index4).toFloat();
  }
