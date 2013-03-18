import serial
import sys
import threading
import twitter
from auth import CONSUMER_KEY, CONSUMER_SECRET, ACCESS_TOKEN, ACCESS_TOKEN_SECRET



SERIAL_PORT = "/dev/ttyUSB0"
TIMEOUT = 600
OPEN_MSG = "Hklbgd je otkljucan."
CLOSE_MSG = "Hklbgd je zakljucan."



def post_status(message):
    api = twitter.Api(consumer_key=CONSUMER_KEY,
            consumer_secret=CONSUMER_SECRET,
            access_token_key=ACCESS_TOKEN,
            access_token_secret=ACCESS_TOKEN_SECRET)
    try:
        api.PostUpdate(message)
        sys.stdout.write("tweet sent - %s " % message)
    except Exception as e:
        sys.stderr.write(str(e))



if __name__ == "__main__":
    timer = None

    srl = serial.Serial(SERIAL_PORT, 9600, timeout=10)

    while True:
        try:
            # read from serial - receive "1" when unlocked, "0" when locked
            unlocked = bool(int(srl.readline()))
        except Exception as e:
            sys.stderr.write(str(e))
            continue

        msg = OPEN_MSG if unlocked else CLOSE_MSG

        if timer is not None and timer.is_alive():
            # if someone changes states 2 times isinde the TIMEOUT
            # then cancel tweeting
            timer.cancel()
            sys.stdout.write("timeout not elapsed - TWITTER ABORT")
        else:
            timer = threading.Timer(TIMEOUT, post_status, args=(msg))
            timer.start() # start timing and tweet when TIMEOUT expires
            sys.stdout.write("timer started, tweeting in %d seconds" % TIMEOUT)
