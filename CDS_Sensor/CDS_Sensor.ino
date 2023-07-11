
#define PIN_CDS 26

void setup() {
  Serial.begin(9600);
  pinMode(PIN_CDS, INPUT);
}

void loop() {
  int CDS_sensor = analogRead(PIN_CDS);
  Serial.print("CDS Value: ");
  Serial.println(CDS_sensor);
  delay(1000);
}
