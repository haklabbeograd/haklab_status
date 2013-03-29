"""
This script and the arduino image should be rewritten to a
request-reply model where the arduino would hold the serial port
and take requests from different clients.
This way more devices could be attached to the arduino and different
clients could read data from the devices connected by querying the arduino.

"""
import time
import serial
import sys
import threading
import twitter
from auth import (CONSUMER_KEY, CONSUMER_SECRET,
        ACCESS_TOKEN, ACCESS_TOKEN_SECRET)



SERIAL_PORT = "/dev/ttyACM0"
TIMEOUT = 600
OPEN_MSG = "Hklbgd je otkljucan."
CLOSE_MSG = "Hklbgd je zakljucan."
ADMINS = []

API = twitter.Api(consumer_key=CONSUMER_KEY,
        consumer_secret=CONSUMER_SECRET,
        access_token_key=ACCESS_TOKEN,
        access_token_secret=ACCESS_TOKEN_SECRET)


def fhwrite(text):
	fh = open("hklbgd.log",'a')
	fh.write(text + "\n")
	fh.close()


def post_status(message):
    try:
        message = time.strftime("%Y-%m-%d %H:%M ") + message
        fhwrite(message)
        API.PostUpdate(message)
        fhwrite("tweet sent - %s " % message)
    except Exception as e:
        fhwrite(str(e))


def send_direct_messages(message):
    try:
        for user in ADMINS:
            API.PostDirectMessage(user, message)
        fhwrite("DM sent - %s " % message)
    except Exception as e:
        fhwrite(str(e))



if __name__ == "__main__":
    timer = None
    counter = 0 # in case of > 3 times fail, send DM to admins

    srl = serial.Serial(SERIAL_PORT, 9600)
    srl.flushInput()
    srl.flushOutput()

    while True:
        try:
            # read from serial - receive "1" when unlocked, "0" when locked
            unlocked = bool(int(srl.readline()))
            fhwrite("read from serial | unlocked=%s" % unlocked)
            counter = 0
        except Exception as e:
            if counter > 3:
                send_direct_messages("FAIL!!! - %s" % e)
                break
            else:
                fhwrite(str(e))
                counter += 1
                continue

        msg = OPEN_MSG if unlocked else CLOSE_MSG

        if timer is not None and timer.is_alive():
            # if someone changes states 2 times isinde the TIMEOUT
            # then cancel tweetpsurd', ' mishkez'ing
            timer.cancel()
            fhwrite("timeout not elapsed - TWITTER ABORT")
        else:
            timer = threading.Timer(TIMEOUT, post_status, args=(msg,))
            timer.start() # start timing and tweet when TIMEOUT expires
            fhwrite("timer started, tweeting in %d seconds" % TIMEOUT)
