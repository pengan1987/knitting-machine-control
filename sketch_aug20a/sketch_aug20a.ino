#include <Servo.h>

int stepPin = 12;
int dirPin = 11;
int enblPin = 10;
int armPin = 9;
char armStatus;

Servo armServo;
int leftEnd = 160;
int rightEnd = 130;
String digitBuffer = "";

void setup()
{
  //Init serial
  Serial.begin(9600);

  //Init servo
  armServo.attach(armPin);
  armServo.write(leftEnd); // set servo to left-point
  armStatus = 'l';

  //Init stepper
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enblPin, OUTPUT);
  digitalWrite(stepPin, LOW);
  digitalWrite(dirPin, LOW);
  digitalWrite(enblPin, LOW);

  Serial.println("\n\nKnitting Machine Ready:");
  Serial.println("\n2021 - Pengan Zhou");
  Serial.println("\nArm control: L to loose, R to tight, Z for repair, M move to specific angle - for debug");
  Serial.println("\nWheel control: [n]S forward n teeth, [n]B to backward n teeth, [n]C to rotate n circle");
  Serial.println("\nAuto hole: [n]H to digging a hole with n teeth width, and automatically sewing");

  Serial.print("\nCommand>");
}

void loop()
{

  // Read serial input:
  while (Serial.available() > 0)
  {
    int inChar = Serial.read();
    long commandNumber = 1;
    if (digitBuffer.length() > 0)
    {
      commandNumber = digitBuffer.toInt();
    }

    if (inChar == 'z' || inChar == 'l' || inChar == 'r')
    {
      setArm(inChar);
    }
    if (inChar == 's')
    {
      wheelForward(commandNumber);
    }
    if (inChar == 'b')
    {
      wheelBack(commandNumber);
    }
    if (inChar == 'c')
    {
      wheelCircle(commandNumber);
    }
    if (inChar == 'h')
    {
      makeHole(commandNumber);
    }
    if (inChar == 'm')
    {
      armStatus = 'r';
      Serial.println("\nmove arm to " + digitBuffer);
      armServo.write(digitBuffer.toInt());
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
      Serial.print("\nCommand>");
    }
  }
}

void wheelForward(long teethNumber)
{
  if (armStatus == 'l' && teethNumber > 5)
  {
    Serial.println("Warning: you can't loose more than 5 steps");
    return;
  }
  Serial.println("wheel forward " + String(teethNumber) + " step");
  rollHandle(256000L * teethNumber / 48L, false);
  delay(500);
}

void wheelBack(long teethNumber)
{

  Serial.println("wheel back " + String(teethNumber) + " step");
  digitalWrite(dirPin, HIGH);
  delay(100);
  rollHandle(256000L * teethNumber / 48L, false);
  digitalWrite(dirPin, LOW);
  delay(500);
}

void wheelCircle(long circleNumber)
{
  if (armStatus == 'l')
  {
    Serial.println("Warning: you can't loose more than 5 steps");
    return;
  }
  Serial.println("wheel forward " + digitBuffer + " circle");
  long count = 0;
  for (count = 0; count < circleNumber; count++)
  {
    bool turbo = (count > 1 && count < circleNumber - 1);
    rollHandle(256000L, turbo);
    Serial.println("Circle done: " + String(count + 1L));
  }
  delay(500);
}

void rollHandle(long steps, bool turbo)
{
  long i;
  int delayNumber = turbo ? 15 : 20;
  Serial.println("roll handle for steps: " + String(steps));
  for (i = 0; i < steps; i++)
  {
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayNumber);
  }
  Serial.println("rolling done");
}

void setArm(char position)
{
  if (position == 'r')
  {
    armStatus = 'r';
    Serial.println("\narm to right - tight");
    armServo.write(rightEnd);
  }
  else if (position == 'z')
  {
    armStatus = 'l';
    Serial.println("\narm to zero - for fix");
    armServo.write(0);
  }
  else
  {
    armStatus = 'l';
    Serial.println("\narm to left - loose");
    armServo.write(leftEnd);
  }
  delay(500);
}

void makeHole(int size)
{
  if (size > 5)
  {
    Serial.println("hole size can't large than 5");
    return;
  }
  Serial.println("digging a hole for size: " + String(size));
  setArm('l');
  delay(500);
  wheelForward(size);
  setArm('r');
  delay(500);
  wheelForward(48 - size - 1);

  int x = 0;
  int sewSize = size / 2 + 2;
  for (x = 0; x < sewSize; x++)
  {
    setArm('l');
    wheelForward(1);
    setArm('r');
    wheelForward(1);
  }
  //sew the tail to make sure everything is closed;
  setArm('r');
  wheelForward(10);
  delay(500);
}