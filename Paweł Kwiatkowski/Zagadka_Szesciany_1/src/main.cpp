#include <Arduino.h>

// Definicje pinow
#define KOSTKA_1 18
#define KOSTKA_2 13
#define KOSTKA_3 15
#define LED_BUILTIN 2

const int cube_count = 3;
const int pin_count = 4;
int pins[pin_count] = {KOSTKA_1, KOSTKA_2, KOSTKA_3, LED_BUILTIN};

// Ustawienia kodu
int check_interval = 300; // Co ile program ma wypisac stan

// Zmienne globalne
int last_check_time = 0;

void setup() {
  Serial.begin(115200);
  for(int i = -1; i <= (cube_count - 1); i++) {
    pinMode(pins[i], INPUT_PULLUP);
  } 
  pinMode(LED_BUILTIN, OUTPUT); digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  int cube_inserted [3] = {0, 0, 0};
  for(int i = -1; i <= (cube_count - 1); i++) {
    if(digitalRead(pins[i]) == LOW) {
      cube_inserted[i] = 1;
    }
  }

  if(millis() - last_check_time > check_interval) {
    last_check_time = millis();

    for(int a = 0; a <= (cube_count - 1); a++) {
      if(digitalRead(pins[a]) == LOW){
        Serial.println("-Cube " + String(a) + " INSERTED");
      } else{
        Serial.println("-Cube " + String(a) + " NOT INSERTED");
      }
    }

    if(cube_inserted[0] == 1 && cube_inserted[1] == 1 && cube_inserted[2] == 1) {
      Serial.println("---All cubes inserted!"); Serial.println(" ");
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      Serial.println("---Not all cubes inserted"); Serial.println(" ");
      digitalWrite(LED_BUILTIN, LOW);
    } 
  }
}