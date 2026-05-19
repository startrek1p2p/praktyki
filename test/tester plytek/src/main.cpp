#include <Arduino.h>

#include <Arduino.h>

const int buttons[] = {13, 27, 26, 25, 32, 33, 15, 2, 4, 16, 17, 21, 22, 23, 19, 18, 5, 34, 35};

const int buttonCount = sizeof(buttons) / sizeof(buttons[0]);

bool lastState[buttonCount];

void setup()
{
  Serial.begin(115200);

  Serial.println();
  Serial.println("=== TEST PRZYCISKOW KOTL SHIELD ===");

  for (int i = 0; i < buttonCount; i++)
  {

    // GPIO34 i GPIO35 nie mają pullupów sprzętowych
    if (buttons[i] == 34 || buttons[i] == 35)
    {
      pinMode(buttons[i], INPUT);
    }
    else
    {
      pinMode(buttons[i], INPUT_PULLUP);
    }

    lastState[i] = digitalRead(buttons[i]);
  }

  Serial.println("Gotowe.");
}

void loop()
{

  for (int i = 0; i < buttonCount; i++)
  {

    bool current = digitalRead(buttons[i]);

    // wykrycie zmiany
    if (current != lastState[i])
    {

      Serial.print("GPIO ");
      Serial.print(buttons[i]);

      if (current == LOW)
      {
        Serial.println(" -> WCISNIETY");
      }
      else
      {
        Serial.println(" -> PUSZCZONY");
      }

      lastState[i] = current;

      delay(20);
    }
  }
}