# Knitting-machine-control

This repo is used for controlling a SENTRO/SANTRO 48 Needles Knitting Machine with steep motor and servo robot arm.

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
- Arm control: L to loosen, R to tight, Z for repair, M move to specific angle - for debug
- Wheel control: [n]S forward n teeth, [n]B to backward n teeth, [n]C to rotate n circle
- Auto hole: [n]H to digging a hole with n teeth width, and automatically sewing

Example:  
5s = forward 5 teeth  
5c = rotate 5 circles

## Punched Card Data Translator
The cardwatcher.py program will translate the punched card data to knitting machine actions. The punched holes are grouped 3 as a group.
- The first option is number of circles lead-in and lead-out (3 circles minimum)
- The second option is the size (number of 1-5 teeth) of hole
- The third option is the offset of next hole (5 teeth as a step)

For example, 3 questions answer "ABC" will translated as:  
(A=1) +2=3 circles lead-in and lead-out  
(B=2) teeth hole  
(C=3) *5=15 teeth offset after hole  
The command will be  
`3c 2h 15s 3c`

## How everything works together?
The scanner software triggers Windows batch file `OMRChecker/PROCESSFILE.bat` then scanned image crop by ImageMagick, and the cropped image of punching area to ORMChecker for recognize, when the result file saved, CardWatcher will catch the new file and convert it to knitting machine actions.
