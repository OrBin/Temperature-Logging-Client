import dht
import machine
import network
import time
import config
import json
import urequests


sta_if = network.WLAN(network.STA_IF)


def connect(timeout_sec=0):
    if sta_if.isconnected():
        return True

    sta_if.active(True)
    sta_if.connect(config.WIFI_SSID, config.WIFI_PASSWORD)
    timeout_end = time.time() + timeout_sec

    while not sta_if.isconnected():
        if time.time() > timeout_end:
            return False

    return True


# Deactivating access point. This didn't work when put inside a function :(
ap = network.WLAN(network.AP_IF)
ap.active(False)

# Initialize DHT22 sensor
dht_sensor = dht.DHT22(machine.Pin(4))

last_connected = time.time()

while True:
    try:
        dht_sensor.measure()
        temperature = dht_sensor.temperature()
        humidity = dht_sensor.humidity()

        json_object = {
            "logger": config.LOGGER_ID,
            "humidity": humidity,
            "temperature_celsius": temperature,
        }

        json_text = json.dumps(json_object)
        print(json_text)

        if not connect(config.CONNECTION_TIMEOUT_SEC):
            print("No connection, Skipping")
            # TODO save data and send later (ensure server support for multiple logs post)

            if (time.time() - last_connected) > config.MAX_ALLOWED_OFFLINE_TIME_SEC:
                machine.reset()

        else:
            response = urequests.post(config.LOGGING_URL,
                                      data=json_text,
                                      headers={"content-type": "application/json"})
            print(response.text)
            last_connected = time.time()

    except OSError as os_error:
        if os_error.args[0] == 110: # ETIMEDOUT
            print("Cannot access sensor: timed out")
        # TODO Handle other errors

    time.sleep(config.MEASUREMENT_INTERVAL_SEC)
