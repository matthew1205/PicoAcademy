
void setup() {
  Serial.begin(9600);
  pinMode(27, INPUT);
}

void loop() {
  int rain_sensor = analogRead(27);

  if( rain_sensor > 900 ){
    Serial.println("Not Raining");
  }
  else{
    Serial.println("Raining");
  }
    
  Serial.print("Rain Sensor Value: ");
  Serial.print(rain_sensor);
  Serial.print(" ");

  delay(1000);
}
