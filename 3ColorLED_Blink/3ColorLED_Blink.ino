const int RED_LED = 7;
const int GREEN_LED = 10;
const int BLUE_LED = 11;

void setup() {
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
}

void loop() {
  digitalWrite(RED_LED, LOW );
  digitalWrite(GREEN_LED, HIGH );
  digitalWrite(BLUE_LED, HIGH );
  delay(1000);
  
  digitalWrite(RED_LED, HIGH );
  digitalWrite(GREEN_LED, HIGH );
  digitalWrite(BLUE_LED, HIGH );
  delay(1000);
}
/*  
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  delay(1000);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, LOW);
  delay(1000);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, HIGH);
  delay(1000);

  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  delay(1000);

}*/
