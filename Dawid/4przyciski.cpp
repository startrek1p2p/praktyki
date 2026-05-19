#include <Arduino.h>
#include <string.h>
#define led1 34
#define zamek 35
#define buttonPin1 32
#define buttonPin2 33
#define buttonPin3 25
#define buttonPin4 26
int odpowiedz[6];
int haslo[6] = {4,3,2,1,2,3};
int nacisniecia = 0;
void setup() {
Serial.begin(115200);
pinMode(led1,OUTPUT);
pinMode(zamek,OUTPUT);
pinMode(buttonPin1, INPUT_PULLUP);
pinMode(buttonPin2, INPUT_PULLUP);
pinMode(buttonPin3, INPUT_PULLUP);
pinMode(buttonPin4, INPUT_PULLUP);  
digitalWrite(zamek,LOW);
}
void loop() {
  for (int  i = 0; i < 6; i++){
  while(digitalRead(buttonPin1) == HIGH && 
        digitalRead(buttonPin2) == HIGH && 
        digitalRead(buttonPin3) == HIGH && 
        digitalRead(buttonPin4) == HIGH) 
        {
        }
  if(digitalRead(buttonPin1) == LOW){
     odpowiedz[i] = 1;
      Serial.println("Wpisano przycisk: 1"); delay(200);
    } else if(digitalRead(buttonPin2) == LOW){
      odpowiedz[i] = 2;
      Serial.println("Wpisano przycisk: 2"); delay(200);
    } else if(digitalRead(buttonPin3) == LOW){
    odpowiedz[i] = 3;
      Serial.println("Wpisano przycisk: 3"); delay(200);
    } else if(digitalRead(buttonPin4) == LOW){
     odpowiedz[i] = 4;
      Serial.println("Wpisano przycisk: 4"); delay(200);
    }
    nacisniecia ++;
    delay (250);
  }
if(nacisniecia == 6)
{
bool correct = true;
for (int k = 0; k < 6; k++) {
          if (odpowiedz[k] != haslo[k]) {
            correct = false;
            break; } }
        if (correct) {
          Serial.println("POPRAWNY KOD");
          digitalWrite(led1,HIGH);
        } else {
          Serial.println("BLEDNY KOD");
          digitalWrite(led1,LOW);
          digitalWrite(zamek,HIGH);
        } 
nacisniecia = 0;
}
}

