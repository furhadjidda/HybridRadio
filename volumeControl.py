from RPi import GPIO
from time import sleep
import subprocess
from subprocess import Popen, PIPE

clk = 5
dt = 6
btn = 13

# vals from output of amixer cget numid=1
min = 0
max = 100

GPIO.setmode(GPIO.BCM)
GPIO.setup(clk, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(dt, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
GPIO.setup(btn, GPIO.IN, pull_up_down=GPIO.PUD_UP)

isMuted = False
preVolume = volume = 0  # give it some volume to start with
clkLastState = GPIO.input(clk)
btnLastState = GPIO.input(btn)

volumeStr = str(volume)+"%"
subprocess.call(['amixer', '-c', '0', 'cset', 'numid=1', volumeStr])

try:
    while True:
        btnPushed = GPIO.input(btn)
        if ((not btnLastState) and btnPushed):
            if isMuted:
                volume = preVolume
                isMuted = False
                print "Unmuted"
            else:
                preVolume = volume
                volume = 0
                isMuted = True
                print "Muted"
            volumeStr = str(volume)+"%"
            subprocess.call(['amixer', '-c', '0', 'cset', 'numid=1', volumeStr])
            sleep(0.10)
        else:
            clkState = GPIO.input(clk)
            dtState = GPIO.input(dt)
            if clkState != clkLastState:
                if isMuted:
                    isMuted = False
                    volume = 0
                if dtState != clkState:
                    volume += 2
                    if volume > max:
                        volume = max
                else:
                    volume -= 2
                    if volume < min:
                        volume = min
                print "{:d} ({:.0%})".format(volume, float(volume)/float(max))
                volumeStr = str(volume)+"%"
                subprocess.call(['amixer', '-c', '0', 'cset', 'numid=1', volumeStr])
            clkLastState = clkState
        btnLastState = btnPushed
finally:
    GPIO.cleanup()
