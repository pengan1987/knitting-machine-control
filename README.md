# knitting-machine-control

This repo is used for control a SENTRO/SANTRO 48 Needles Knitting Machine with steep motor and servo robot arm.

## Hardware setup
- SENTRO/SANTRO 48 Needles Knitting Machine
- Rtelligent 57CM30 Stepper motor
- Rtelligent DM542 Stepper controller
- Arudino Uno
- DS3225MG Digital servo

## Arduino Pinout
- Pin 9 - Servo signal
- Pin 10 - Stepper ENA
- Pin 11 - Stepper DIR
- Pin 12 - Stepper PULSE

## Serial Command
All command should be lower case
- Arm control: L to loose, R to tight, Z for repair, M move to specific angle - for debug
- Wheel control: [n]S forward n teeth, [n]B to backward n teeth, [n]C to rotate n circle
- Auto hole: [n]H to digging a hole with n teeth width, and automatically sewing

Example:  
5s = forward 5 teeth  
5c = rotate 5 circle
