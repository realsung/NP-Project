import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522
from time import sleep
from amqtt.client import MQTTClient, ConnectException, ClientException
from amqtt.mqtt.constants import QOS_1
import logging
import asyncio


# initialize
logger = logging.getLogger(__name__)
reader = SimpleMFRC522()

# Pins setup
servoPIN = 26
RLED = 5
GLED = 6
speakerPIN = 12
GPIO.cleanup()
GPIO.setmode(GPIO.BCM)
GPIO.setup(servoPIN, GPIO.OUT)
GPIO.setup(RLED, GPIO.OUT)
GPIO.setup(GLED, GPIO.OUT)
GPIO.setup(speakerPIN, GPIO.OUT)

# Initialize PWM on pwmPIN with Frequency = 50Hz
pwm = GPIO.PWM(servoPIN, 50)
pwm.start(0)

# tones
tones = [661, 525, 589, 786]
tones2 = [1397, 1397, 1397, 1397]

def set_angle(angle):
    duty = angle / 18 + 2
    GPIO.output(servoPIN, True)
    pwm.ChangeDutyCycle(duty)
    sleep(1)
    GPIO.output(servoPIN, False)
    pwm.ChangeDutyCycle(0)

def play_tone(tone, duration):
    buzzer = GPIO.PWM(speakerPIN, tone)
    buzzer.start(50)
    sleep(duration)
    '''
    for i in range(int(duration*1000)):
        GPIO.output(speakerPIN, GPIO.HIGH)
        sleep(tone/1000000.0)
        GPIO.output(speakerPIN, GPIO.LOW)
        sleep(tone/1000000.0)
    '''

async def uptime_coro():
    C = MQTTClient()
    await C.connect("mqtt://door:doorpass@homeofangel.duckdns.org:18833") # test:test@homeofangel.duckdns.org:18833
    # await C.connect('mqtt://0.0.0.0:1883')
    # Subscribe to '$SYS/broker/uptime' with QOS=1
    await C.subscribe(
        [
            ("door/unlock", QOS_1),  # Topic allowed
            # ("data/classified", QOS_1),  # Topic forbidden
            # ("repositories/amqtt/master", QOS_1),  # Topic allowed
            # ("repositories/amqtt/devel", QOS_1),  # Topic forbidden
            # ("calendar/amqtt/releases", QOS_1),  # Topic allowed
        ]
    )
    logger.info("Subscribed")
    try:
        while True:
            message = await C.deliver_message()
            packet = message.publish_packet
            print(
                "%s => %s"
                % (packet.variable_header.topic_name, str(packet.payload.data))
            )
            if(packet.payload.data == b'open'):
                print("Authorized access")
                GPIO.output(GLED, GPIO.HIGH)
                set_angle(0)

                for tone in tones:
                    play_tone(tone, 0.3)

                sleep(3)
                set_angle(180)
                GPIO.output(GLED, GPIO.LOW)
    except ClientException as ce:
        logger.error("Client exception: %s" % ce)


if __name__ == '__main__':
    print("Doorlock Sensor!")
    print("Network Programming Challenge")
    print("Hold a tag near the reader")
    formatter = (
        "[%(asctime)s] %(name)s {%(filename)s:%(lineno)d} %(levelname)s - %(message)s"
    )
    formatter = "%(message)s"
    logging.basicConfig(level=logging.INFO, format=formatter)
    asyncio.get_event_loop().run_until_complete(uptime_coro())