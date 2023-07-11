#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup(){
  Serial.begin(9600);
  dht.begin();
}

void loop(){
  //Wait a few seconds between measurements
  delay(2000);
  
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  float F = dht.readTemperature(true);

  //Check if any reads failed and exit early (to try again)
  if(isnan(hum) || isnan(temp) || isnan(F)){
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" % ");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("C");
}
