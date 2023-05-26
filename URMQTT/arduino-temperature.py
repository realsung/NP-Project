import serial
import logging
import asyncio

from amqtt.client import MQTTClient, ConnectException, ClientException
from amqtt.mqtt.constants import QOS_1


#
# This sample shows how to publish messages to broker using different QOS
# Debug outputs shows the message flows
#

logger = logging.getLogger(__name__)

'''
async def uptime_coro():
    C = MQTTClient()
    await C.connect("mqtt://test:test@homeofangel.duckdns.org:18833") # test:test@homeofangel.duckdns.org:18833
    # await C.connect('mqtt://0.0.0.0:1883')
    # Subscribe to '$SYS/broker/uptime' with QOS=1
    await C.subscribe(
        [
            ("temperature/get", QOS_1),  # Topic allowed
        ]
    )
    logger.info("Subscribed")
    try:
        for i in range(1, 100):
            message = await C.deliver_message()
            packet = message.publish_packet
            print(
                "%d: %s => %s"
                % (i, packet.variable_header.topic_name, str(packet.payload.data))
            )

        await C.unsubscribe(["$SYS/broker/uptime", "$SYS/broker/load/#"])
        # logger.info("UnSubscribed")
        # await C.disconnect()
    except ClientException as ce:
        logger.error("Client exception: %s" % ce)
'''
async def test_coro():
	C = MQTTClient()
	await C.connect("mqtt://temperature:temperaturepass@homeofangel.duckdns.org:18833")
	while True:
		try:
			ser = serial.Serial(serial_port, baud_rate)
			data = ser.readline().decode().strip().encode()
			print(data)
			await C.publish("temperature/get", data, qos=0x01)
			logger.info("messages published")
		except ConnectException as ce:
			logger.error("Connection failed: %s" % ce)
			await C.disconnect()
			asyncio.get_event_loop().stop()


if __name__ == "__main__":
	formatter = (
		"[%(asctime)s] %(name)s {%(filename)s:%(lineno)d} %(levelname)s - %(message)s"
	)
	formatter = "%(message)s"
	logging.basicConfig(level=logging.INFO, format=formatter)
	# asyncio.get_event_loop().run_until_complete(uptime_coro())
	# 시리얼 포트 설정
	serial_port = '/dev/cu.usbmodem144101'
	baud_rate = 9600  # 시리얼 통신 속도

	# 시리얼 포트 열기
	# ser = serial.Serial(serial_port, baud_rate)

	try:
		asyncio.get_event_loop().run_until_complete(test_coro())

	except KeyboardInterrupt:
		# Ctrl+C를 누르면 프로그램 종료
		ser.close()
		print("프로그램을 종료합니다.")
