#include <Servo.h> 

Servo myservo;
int leftEnd =170;
int rightEnd=130;

void setup() 
{ 
    Serial.begin(9600);

  myservo.attach(9);
  myservo.write(300);  // set servo to mid-point
    Serial.println("\n\nString toInt():");

} 

String inString = ""; 
void loop() {
  // Read serial input:
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char
      // and add it to the string:
      inString += (char)inChar;
    }
    // if you get a newline, print the string,
    // then the string's value:
    if (inChar == '\n') {
      Serial.print("Value:");
      Serial.println(inString.toInt());
      Serial.print("String: ");
      Serial.println(inString);

      for(int x=0;x<inString.toInt();x++){
        myservo.write(leftEnd);
        delay(1000);
                myservo.write(rightEnd);
        delay(1000);
      }

   
      // clear the string for new input:
      inString = "";
    }
  }
}
