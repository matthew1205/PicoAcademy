
const int Relay_Pin = 22;

void setup() {
  Serial.begin(115200);
  pinMode(Relay_Pin, OUTPUT);
  Serial.printf("Simple Relay Operation Example\r\n");
}

void loop() {
  
  digitalWrite(Relay_Pin, HIGH);    //Relay ON, You can hear "tick"
  Serial.printf("Relay ON, COM Pin wired NO Pin \r\n");
  delay(1000);

  digitalWrite(Relay_Pin, LOW);     //Relay OFF
  Serial.printf("Relay OFF, COM Pin wired NC Pin \r\n");
  delay(1000);
  
}
