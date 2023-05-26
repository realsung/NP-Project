import RPi.GPIO as GPIO
from mfrc522 import SimpleMFRC522
from time import sleep
from amqtt.client import MQTTClient, ConnectException, ClientException
from amqtt.mqtt.constants import QOS_1

# initialize
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
while True:
    print("Doorlock Sensor!")
    print("Network Programming Challenge")
    print("Hold a tag near the reader")

    id = reader.read_id()
    print(f'Tag ID : {id}')
    if id == 692512562033:
        print("Authorized access")
        GPIO.output(GLED, GPIO.HIGH)
        set_angle(0)

        for tone in tones:
            play_tone(tone, 0.3)

        sleep(3)
        set_angle(180)
        GPIO.output(GLED, GPIO.LOW)
    else:
        print("Access denied")
        GPIO.output(RLED, GPIO.HIGH)
        for tone in tones2:
            play_tone(tone, 0.1)
        sleep(3)
        GPIO.output(RLED, GPIO.LOW)