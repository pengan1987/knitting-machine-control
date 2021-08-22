#include <Servo.h>

int stepPin = 12;
int dirPin = 11;
int enblPin = 10;
int armPin = 9;
char armStatus;

Servo myservo;
int leftEnd = 170;
int rightEnd = 130;
String digitBuffer = "";

void setup()
{
  //Init serial
  Serial.begin(9600);

  //Init servo
  myservo.attach(armPin);
  myservo.write(leftEnd); // set servo to left-point
  armStatus = 'l';

  //Init stepper
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enblPin, OUTPUT);
  digitalWrite(stepPin, LOW);
  digitalWrite(dirPin, LOW);
  digitalWrite(enblPin, LOW);

  Serial.println("\n\nKnitting Machine Ready:");
  Serial.print("\nCommand>");
}

void loop()
{

  // Read serial input:
  while (Serial.available() > 0)
  {
    int inChar = Serial.read();
    long commandTimes = 1;
    if (digitBuffer.length() > 0)
    {
      commandTimes = digitBuffer.toInt();
    }
    if (inChar == 'z')
    {
      armStatus = 'l';
      Serial.println("arm to zero - for fix");
      myservo.write(0);
    }
    if (inChar == 'l')
    {
      armStatus = 'l';
      Serial.println("arm to left - loose");
      myservo.write(leftEnd);
    }
    else if (inChar == 'r')
    {
      armStatus = 'r';
      Serial.println("arm to right - tight");
      myservo.write(rightEnd);
    }
    else if (inChar == 's')
    {
      if (armStatus == 'l' && commandTimes > 5)
      {
        commandTimes = 5;
        Serial.println("Warning: you can't loose more than 5 steps");
      }
      Serial.println("wheel forward " + digitBuffer + " step");
      rollHandle(256000L / 48L * commandTimes);
    }
    else if (inChar == 'b')
    {
      Serial.println("wheel back " + digitBuffer + " step");
      digitalWrite(dirPin, HIGH);
      delay(100);
      rollHandle(256000L / 48L * commandTimes);
      digitalWrite(dirPin, LOW);
      delay(100);
    }
    else if (inChar == 'c')
    {
      if (armStatus == 'r')
      {
        Serial.println("wheel forward " + digitBuffer + " circle");
        rollHandle(256000L * commandTimes);
      }
      else
      {
        Serial.println("Warning: you can't loose more than 5 steps");
      }
    }
    if (isDigit(inChar))
    {
      //Add new digit into digit buffer
      digitBuffer += (char)inChar;
    }
    else
    {
      //After machinecal action, wait for 1sec, clean digit buffer
      digitBuffer = "";
      delay(500);
      Serial.print("\nCommand>");
    }
  }
}

void rollHandle(long steps)
{
  long i;
  Serial.println("roll handle for steps: " + String(steps));
  for (i = 0; i < steps; i++)
  {
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(20);
  }

  Serial.println("rolling done");
}
