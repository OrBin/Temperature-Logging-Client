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


def compute_heat_index(temperature_c, humidity):
    # Heat-Index calculator with celsius-grade
    # Formulas are at https://en.wikipedia.org/wiki/Heat_index#Formula
    # GregNau	2015
    # https://github.com/gregnau/heat-index-calc

    temperature_f = ((temperature_c * 9/5) + 32)

    # Creating multiples of 'fahrenheit' & 'hum' values for the coefficients
    T2 = pow(temperature_f, 2)
    H2 = pow(humidity, 2)

    # Coefficients for the calculations
    C1 = [ -42.379, 2.04901523, 10.14333127, -0.22475541, -6.83783e-03, -5.481717e-02, 1.22874e-03, 8.5282e-04, -1.99e-06]

    # Calculating heat-indexes with 3 different formula
    heatindex_f = C1[0] + (C1[1] * temperature_f) + (C1[2] * humidity) + (C1[3] * temperature_f * humidity) + (C1[4] * T2) + (C1[5] * H2) + (C1[6] * T2 * humidity) + (C1[7] * temperature_f * H2) + (C1[8] * T2 * H2)

    return round(((heatindex_f - 32) * 5/9), 2)


# Initialize DHT22 sensor
dht_sensor = dht.DHT22(machine.Pin(4))

last_connected = time.time()

while True:
    try:
        dht_sensor.measure()
        temperature = dht_sensor.temperature()
        humidity = dht_sensor.humidity()
        heat_index = compute_heat_index(temperature, humidity)

        json_object = {
            "logger": config.LOGGER_ID,
            "humidity": humidity,
            "temperature_celsius": temperature,
            "heat_index_celsius": heat_index
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
