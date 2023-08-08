#include <WiFi.h>
#include "ThingSpeak.h"
#include "DHT.h"
#define DHTPIN 4     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
const int moisturePin=A0;
const int motorPin=26;

const char* ssid = "POCO X4 5G";   // your network SSID (name) 
const char* password = "885dfbad";   // your network password
WiFiClient  client;

unsigned long myChannelNumber = 1561905;
const char * myWriteAPIKey = "Z36QIP4REJ0GSKV8";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Variable to hold temperature readings
float temperatureC;
float Humidity;


void setup() {
  Serial.begin(115200);  //Initialize serial
  //initBME();
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  dht.begin();
  pinMode(motorPin,OUTPUT);
  digitalWrite(motorPin,LOW);

}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }


      delay(2000);
    
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      float f = dht.readTemperature(true);
        
      int moisturePercentage=(100.00-((analogRead(moisturePin)/4096.00)*100));
      int motorStatus=0;
      Serial.print(F("Humidity: "));
      Serial.print(h);
      Serial.print(F("Temerature: "));
      Serial.print(t);
      Serial.print(F("Celsius: "));
      Serial.print(f);
      Serial.println(F("F:"));
      Serial.println(moisturePercentage);

      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) 
      {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
      }
      else
      {

        ThingSpeak.setField(1, h);
        ThingSpeak.setField(2, t);
        ThingSpeak.setField(3, moisturePercentage);

        if(moisturePercentage==0){
          motorStatus=1;
          digitalWrite(motorPin,HIGH);
        }
        else{
          motorStatus=0;
          digitalWrite(motorPin,LOW);
        }
        ThingSpeak.setField(4, motorStatus);
        
        int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        if(x == 200){
          Serial.println("Channel update successful.");
        }
        else{
          Serial.println("Problem updating channel. HTTP error code " + String(x));
        }
        lastTime = millis();
      }
    

    
      

  
    
    
  }
}
