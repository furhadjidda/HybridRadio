#!/usr/bin/env python3

from time import sleep
import RPi.GPIO as GPIO
from ky040.KY040 import KY040
import sys
import time
import subprocess
from subprocess import Popen, PIPE

CLOCKPIN = 5
DATAPIN = 6
SWITCHPIN = 13


class Volume:

    def __init__(self):
        self.isMuted = False
        self.volume = 40
        self.preVolume = 40
        self.min = 20
        self.max = 100

    def rotaryChange(self,direction):
        print("turned - " + str(direction))
        volumeStr = ""
        print(self.volume)
        if direction == KY040.ANTICLOCKWISE:
            self.volume += 10
            if self.volume > self.max:
                self.volume = self.max
        elif direction == KY040.CLOCKWISE:
            self.volume -= 10
            if self.volume < self.min:
                self.volume = self.min

        volumeStr = str(self.volume)+"%"
        print "{:d} ({:.0%})".format(self.volume, float(self.volume)/float(self.max))
        subprocess.call(['amixer', '-c', '0', 'cset', 'numid=1', volumeStr])

    def switchPressed(self):
        print("button pressed")
        if self.isMuted:
            self.volume = self.preVolume
            self.isMuted = False
            print "Unmuted"
        else:
            self.preVolume = self.volume
            self.volume = 0
            self.isMuted = True
            print "Muted"
        volumeStr = str(self.volume)+"%"
        subprocess.call(['amixer', '-c', '0', 'cset', 'numid=1', volumeStr])
        sleep(0.10)


vol = Volume()
GPIO.setmode(GPIO.BCM)

ky040 = KY040(CLOCKPIN, DATAPIN, SWITCHPIN, vol.rotaryChange, vol.switchPressed)

ky040.start()

try:
    while True:
        sleep(0.1)
finally:
    ky040.stop()
    GPIO.cleanup()
