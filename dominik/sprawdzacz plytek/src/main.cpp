#include <Arduino.h>
#include <string.h>


// put function declarations here:
int D_porty[14] = {4, 13, 15, 18, 19, 21, 23, 25, 26, 27, 32, 33, 34, 35};
int odczytanie = 0;
void setup() {
  Serial.begin(115200);
  Serial.print("test sprawdzacza");

  for (int i = 0; i < 14; i++) {
    pinMode(D_porty[i], INPUT_PULLUP);
  }
}

void loop() {
  for (int odczytanie = 0; odczytanie < 14; odczytanie++) {
    int odczyt = digitalRead(D_porty[odczytanie]);
     if (odczyt == HIGH) {
      Serial.print("Port D ");
      Serial.print(D_porty[odczytanie]);
      Serial.println(" dziala (HIGH)");
    } else {
      Serial.print("Port D ");
      Serial.print(D_porty[odczytanie]);
      Serial.println(" niedziala (LOW)");
    }
  }
  }



