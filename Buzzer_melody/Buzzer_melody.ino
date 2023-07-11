const int buzzer = 6;

int melody[] = {262, 294, 330, 349, 392, 440, 494, 523};

void setup() {
  pinMode(buzzer, OUTPUT); 
}

void loop() {

  int i = 0;

  for(i = 0; i < 8; i++)
  {
    tone(buzzer, melody[i]);
    delay(500);
    noTone(buzzer);
    delay(500);
  }
  
  for(i = 0; i < 8; i++)
  {
    tone(buzzer, melody[7-i]);
    delay(500);
    noTone(buzzer);
    delay(500);
  }
}
