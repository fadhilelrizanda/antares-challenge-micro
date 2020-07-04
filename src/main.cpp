#include<Arduino.h>
#include<SPI.h>
#include<Wire.h>

// dht 11 configurations
#include "DHT.h"
#define DHTPIN D8     
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
//

//actuator configuration
#define LED D5
#define fan D7
#define Pump D6
//

//soil Moisture configuration
#define soilMoisture A0
int moistureValue;

//Setting Up value from antares
int ledState;
int pumpState;
int fanState;
float limitTemp;
float limitMoist;

//antares configuration
#include<AntaresESP8266HTTP.h>   // Inisiasi library HTTP Antares

#define ACCESSKEY "017c7e810b75e05b:0932f32db0c81348"       // Ganti dengan access key akun Antares anda
#define WIFISSID "TP-LINK 2"         // Ganti dengan SSID WiFi anda
#define PASSWORD "asdf1234"     // Ganti dengan password WiFi anda

#define projectName "antaresChallenge"   // Ganti dengan application name Antares yang telah dibuat
#define deviceNameSend "sensors"     // Ganti dengan device Antares yang telah dibuat
AntaresESP8266HTTP antares(ACCESSKEY);
//
#define deviceNameRead "actuatorState"

//lcd
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7);  // Set the LCD I2C address

void setup() {

  //lcd
    lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);
  lcd.home ();  // go home
  lcd.print("Setting Up");
  lcd.setCursor(0,1);
  lcd.print("Configuration");
  //antares setup
  antares.setDebug(true);   // Nyalakan debug. Set menjadi "false" jika tidak ingin pesan-pesan tampil di serial monitor
  antares.wifiConnection(WIFISSID,PASSWORD);  // Mencoba untuk menyambungkan ke WiFi
  //

  pinMode(LED,OUTPUT);
  pinMode(Pump,OUTPUT);
  pinMode(fan,OUTPUT);
  Serial.begin(9600);

  //dht setup
  dht.begin();

}

void loop() {
  lcd.clear();
  lcd.print("Connecting");
  lcd.setCursor(0,1);
  lcd.print("To antares");
// Get data from antares
antares.get(projectName, deviceNameRead);
  
 if(antares.getSuccess()) {
   // Get each values
   lcd.clear();
   lcd.print("Connected!");
   lcd.setCursor(0,1);
   lcd.print("To antares");
   ledState = antares.getInt("LED");
   fanState = antares.getInt("Fan");
   pumpState = antares.getInt("Pump");
   limitTemp = antares.getFloat("limitTemp");
   limitMoist = antares.getFloat("limitMoisture"); 
   delay(2000);
   lcd.clear();
}
//Read DHT 11
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

//Read Moisture
 moistureValue = analogRead(soilMoisture);
 moistureValue = map(moistureValue, 0, 1024, 0, 100);

  Serial.println("Humiditiy: "+String(h)+"%");
  Serial.println("Temperature: "+String(t) + "C");
  Serial.println(moistureValue);

  antares.add("Humidity",h);
  antares.add("Temperature",t);
  antares.add("Moisture",moistureValue);

lcd.print("Humid: "+String(h)+"%");
lcd.setCursor(0,1);
lcd.print("Temp : "+ String(t)+"C");
delay(4000);
lcd.clear();
lcd.print("Moist :  "+String(moistureValue)+"%");
delay(3000);
lcd.clear();

if(fanState==1){
lcd.print("Temp L: "+String(limitTemp));

}
else{
  lcd.print("Temp L: OFF");
}
lcd.setCursor(0,1);
if(pumpState==1){
lcd.print("Moist L: "+String(limitMoist));
delay(3000);
}
else{
  lcd.print("Moist L: OFF");
  delay(3000);
}
lcd.clear();
if (ledState==1){
  lcd.print("LED: ON");
}
else{
  lcd.print("LED: OFF");
}
lcd.setCursor(0,1);
if(fanState==1){
lcd.print("Fan: ON");
}
else{
  lcd.print("Fan: OFF");
}
delay(3000);
lcd.clear();
if(pumpState==1){
  lcd.print("Pump: ON");
}
else{
   lcd.print("Pump: OFF");
}
delay(2000);
lcd.clear();
lcd.print("Setting");
lcd.setCursor(0,1);
lcd.print("Actuators");
//setting up actuator
if(ledState==1){
    digitalWrite(LED,HIGH);
  }
  else{
    digitalWrite(LED,LOW);
  }

if(fanState==1){
  if(t>=limitTemp){
    digitalWrite(fan,HIGH);
    
  }

  else{
    digitalWrite(fan,LOW);
  }

 }
 else{
   digitalWrite(fan,LOW);
 }
if(pumpState==1){
  if(moistureValue >= limitMoist)
  {
    digitalWrite(Pump,HIGH);
    delay(5000);
    digitalWrite(Pump,LOW); 
  }
  else{
    digitalWrite(Pump,LOW);
    delay(5000);
  }
}
else{
  digitalWrite(Pump,LOW);
}
delay(1000);
lcd.clear();
lcd.print("Sending Data");
lcd.setCursor(0,1);
lcd.print("To antares");
  //sending Value To Antares
  antares.send(projectName, deviceNameSend);
delay(1000);
lcd.clear();
lcd.print("Success!");
 delay(2000);
  
}