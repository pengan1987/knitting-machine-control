
from os import write
import time
import json
import serial
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

arduinoPort = serial.Serial("COM7", baudrate=9600, timeout=3.0)


class MyHandler(FileSystemEventHandler):
    def on_modified(self, event):
        print(f'event type: {event.event_type}  path : {event.src_path}')
        if (event.src_path.endswith('.json')):
            with open(event.src_path) as jsonfile:
                data = json.load(jsonfile)
                print(data)
                commands = translateJson(data)
                print(commands)
                sendCommandSeq(commands)


def translateJson(data):
    blankCounter = 0
    result = []
    options = []
    valueDict = {'A': 1, 'B': 2, 'C': 3, 'D': 4, 'E': 5}
    for key in data:
        val = data[key]
        if val == 'ABCDE' or len(val) == 0:
            blankCounter = blankCounter + 1
        else:
            options.append(val[0])

    while len(options) > 0:
        leadInout = valueDict[options.pop(0)] + 2
        result.append(str(leadInout)+"c")
        holeSize = 0
        leadOutSteps = 12
        if len(options) > 0:
            holeSize = valueDict[options.pop(0)]
            result.append(str(holeSize)+"h")
        if len(options) > 0:
            leadOutSteps = valueDict[options.pop(0)]*5
            result.append(str(leadOutSteps)+"s")
        result.append(str(leadInout)+"c")

    if (len(data.keys()) == blankCounter):
        print('this is blank!')
        return []
    else:
        return result


def sendCommandSeq(commands):
    for command in commands:
        arduinoPort.write(command.encode())
        rcv = ""
        while not rcv == ">":
            rcv = arduinoPort.read(1).decode('ascii', 'ignore')

            print(rcv, end="")


if __name__ == "__main__":
    print("set up Arduino")
    sendCommandSeq(["\n\n", "r"])

    event_handler = MyHandler()
    observer = Observer()
    observer.schedule(
        event_handler, path='C:\Projects\OMRChecker\outputs\MySample\Results', recursive=False)
    observer.start()

    print("Start watching JSON files")
    try:
        while True:
            time.sleep(5)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()
