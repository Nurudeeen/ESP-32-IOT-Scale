#include "HX711.h"
#include <ArduinoJson.h>
#include <Arduino_JSON.h>

#include <HTTPClient.h>
#include <WiFiMulti.h>

#include <Wire.h>
#include "soc/rtc.h"

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);
 

const char *AP_SSID = "Fly";
const char *AP_PWD = "oluwaburna";

WiFiMulti wifiMulti; 


const int DOUT = 25;
const int CLK = 26;
HX711 scale;

#include <Preferences.h>
Preferences preferences;

 
int rbutton = 27; // this button will be used to reset the scale to 0.
int ebutton = 32;
int fbutton = 33;

float FW;
float CW;
float EW;

float percent = ((CW-EW)/(FW-EW))*100;

float calibration_factor = 69995; // for me this vlaue works just perfect 419640
 
void setup() 
{
 rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

 lcd.begin(20,4);
 lcd.init();
 lcd.backlight();
 Serial.begin(9600);
 
  //delay(4000);
  wifiMulti.addAP(AP_SSID, AP_PWD);
 
  scale.begin(DOUT,CLK);
 pinMode(rbutton, INPUT_PULLUP);
  pinMode(ebutton, INPUT_PULLUP);
  pinMode(fbutton, INPUT_PULLUP);
  lcd.setCursor(4,0);
  lcd.print("Welcome");
  lcd.setCursor(2,2);
  lcd.print("IOT Gas Scale");
   delay(5000);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  lcd.clear();
  
}
 
void loop() 
 
{
  preferences.begin("weight", false);
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
 
  CW = scale.get_units(2);
  CW= abs(CW); 
  EW = preferences.getFloat("EW", 0); 
  FW = preferences.getFloat("FW", 5);
  float percent = ((CW-EW)/(FW-EW))*100;
  percent = abs (percent);

Serial.println(EW);
Serial.println(FW);
Serial.println(percent);
lcd.setCursor(0,0);
lcd.print("Weight: ");
lcd.print(CW);
lcd.print(" KG");
lcd.setCursor(0,1);
lcd.print("Gas Level: ");;
lcd.setCursor(0,2);
lcd.print(percent);
lcd.print(" Percent");

  delay(1500);
  //lcd.clear();
  
  Serial.print("Weight: ");
  Serial.print(CW);
  Serial.println(" KG");
  Serial.println();
 
  if (wifiMulti.run() == WL_CONNECTED) {
     
    HTTPClient http;   
     
    http.begin("https://tawheed-final-year-project.herokuapp.com/1/updateVolume/");  
    http.addHeader("Content-Type", "application/json");         
    http.addHeader("token", "130bf0261-ebc2-4e8a-b1fb-1dde5e4b46751"); 
    StaticJsonDocument<200> doc;
//    String CW = String(CW, 2);
//    String EW = String(EW, 2);
//    String FW = String(FW, 2);
    //                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        String percent = String(percent, 2);
    doc[ "current_weight"] = CW;
    doc["full_weight"] = FW;
    doc["empty_weight"] = EW;
    doc[ "volume_in_percent"] = percent;
    doc["mode"] = "non-reset";
    if (digitalRead(ebutton) == LOW)
          {
            lcd.clear();
            lcd.print("Measure Empty Weight");
            delay(5000);
            Serial.println("Measure Empty Weight");
            EW = scale.get_units(2);
            EW=abs(EW);
            lcd.print(EW);
            lcd.print(" KG");
            Serial.println(EW);
            delay(3000);
            lcd.clear();
            preferences.putFloat("EW", EW);
            lcd.print("Empty Weight Saved");
            delay(3000);
            lcd.clear();
          }
          else if (digitalRead(fbutton) == LOW)
          {
            lcd.clear();
            lcd.print("Measure Full Weight");
            Serial.println("Measure Full Weight");
            delay(8000);
            FW = scale.get_units(2);
            FW=abs(FW);
            lcd.setCursor(0,1);
            lcd.print(FW);
            lcd.print(" KG");
            delay(3000);
            lcd.clear();
            Serial.println(FW);
            //delay(3000);
            preferences.putFloat("FW", FW);
            lcd.print("Full Weight Saved");
            delay(3000);
            lcd.clear();
            CW = EW;
            }
             else if (digitalRead(rbutton) == LOW)
          {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Empty Weight: ");
            lcd.setCursor(0,1);
            lcd.print(EW);
            lcd.print(" KG");
            lcd.setCursor(0,2);
            lcd.print("Full Weight: ");
            lcd.setCursor(0,3);
            lcd.print(FW);
            lcd.print(" KG");
            delay(5000);
            lcd.clear();
            lcd.print("Updating Server");
            delay(3000);
            float percent = ((CW-EW)/(FW-EW))*100;
            percent = abs (percent);

            Serial.println("Initializing post to server...please wait");
//            lcd.print("Initializing post..");
//            delay(2000);
            HTTPClient http;   
            http.begin("https://tawheed-final-year-project.herokuapp.com/1/updateVolume/");  
            http.addHeader("Content-Type", "application/json");         
            http.addHeader("token", "130bf0261-ebc2-4e8a-b1fb-1dde5e4b46751");
            StaticJsonDocument<200> doc;
//            String CW = String(CW, 2);
//            String EW = String(EW, 2);
//            String FW = String(FW, 2);
            //String percent = String(percent, 2);
            doc["current_weight"] = CW;
            doc["full_weight"] = FW;
            doc["empty_weight"] = EW;
            doc["volume_in_percent"] = percent;
            doc["mode"] = "reset";
            String resetBody;
            serializeJson(doc, resetBody);
             
            int httpResponseCode = http.POST(resetBody);
            preferences.end();
            if(httpResponseCode>0){
               
            String response = http.getString();                       
               
            Serial.println(httpResponseCode);   
            Serial.println(response);
            lcd.clear();
            lcd.print("Server Reset Done");
            delay(3000);
            lcd.clear();
          }
            
          }else{
    String requestBody;
    serializeJson(doc, requestBody);
    if (CW>0.20){
    int httpResponseCode = http.POST(requestBody);
    if(httpResponseCode>0){
       
      String response = http.getString();                       
       
      Serial.println(httpResponseCode);   
      Serial.println(response);
    }
    lcd.setCursor(0,3);
    lcd.print("Server Updated");
    }
    }
  }
}
