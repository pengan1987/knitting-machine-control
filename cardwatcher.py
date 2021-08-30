
import os
import glob
import time
import json
import serial
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler

arduinoPort = serial.Serial("COM3", baudrate=9600, timeout=3.0)
cardExpire = 300
workingPath = "D:\DotTextTile\OMRChecker\outputs\PunchReader\Results"


class MyHandler(FileSystemEventHandler):
    def on_modified(self, event):
        print(f'event type: {event.event_type}  path : {event.src_path}')
        if os.path.exists(event.src_path) and event.src_path.endswith('.json'):
            locklife = setupFileLock()
            print("Already working for secs: "+str(locklife))
            if locklife < cardExpire:

                with open(event.src_path) as jsonfile:
                    data = json.load(jsonfile)
                    print(data)
                    commands = translateJson(data)
                    print(commands)
                    sendCommandSeq(commands)
                os.remove(event.src_path)
            else:
                print("card too old, drop all rest card files")
                files = glob.glob(workingPath + '\*.json')
                for file in files:
                    os.remove(file)
            setupFileLock()


def setupFileLock():
    # The lock file is used for prevent too much card scanned at once and 
    # the knitting machine keeps busy and not response new cards, when new 
    # card scanned (new json created) and knitting machine starts running a
    # lock file created and used for tracking how long the knitting machine
    # is running
    # When the machine running time is longer than cardExpire time, current card will
    # be knitted and following card will be deleted, the knitting machine will stop
    # work and waiting for new cards.
    # If all scanned card have been knitted with in cardExpire time, the lock file
    # will be deleted and waiting for new cards.
    lockPath = workingPath+'\lock.lck'
    lockExists = os.path.exists(lockPath)
    jsonExists = glob.glob(workingPath + '\*.json')

    if not lockExists:
        with open(lockPath, mode='w') as lockFile:
            lockFile.write('This is used for detect card file life')
    if not jsonExists:
        os.remove(lockPath)
    if lockExists and jsonExists:
        return time.time() - os.path.getmtime(lockPath)
    else:
        return 0


def translateJson(data):
    blankCounter = 0
    result = []
    options = []
    #valueDict = {'A': 1, 'B': 2, 'C': 3, 'D': 4, 'E': 5}
    for key in data:
        val = data[key]
        if val == "01234567891011" or len(val) == 0:
            blankCounter = blankCounter + 1
        else:
            intVal = int(val)
            options.append(intVal % 5+1)

    while len(options) > 0:
        leadInout = options.pop(0) + 2
        result.append(str(leadInout)+"c")
        holeSize = 0
        leadOutSteps = 12
        if len(options) > 0:
            holeSize = options.pop(0)
            result.append(str(holeSize)+"h")
        if len(options) > 0:
            leadOutSteps = options.pop(0)*5
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
    files = glob.glob(workingPath + '\*.*')
    for file in files:
        os.remove(file)
    print("set up Arduino")
    sendCommandSeq(["\n\n", "r"])

    event_handler = MyHandler()
    observer = Observer()
    observer.schedule(
        event_handler, path=workingPath, recursive=False)
    observer.start()

    print("Start watching JSON files")
    try:
        while True:
            time.sleep(5)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()
