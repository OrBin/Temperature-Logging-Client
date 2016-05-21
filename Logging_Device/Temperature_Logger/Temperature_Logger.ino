/**
 *                               Temperature Logger for NodeMCU
 * 
 * -----------------------------------------------------------------------------------------
 * 
 * This code tests the temperature, humidity and heat index once a minute using DHT11 sensor,
 * and send it to a server to save the data for future use.
 * 
 * This code is base on BasicHTTPClient.ino example which comes with NodeMCU library, 
 * and DHTtester example which comes with DHT sensor library.
 */
 
#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

#include "DHT.h"
#include <ctime>

#include "Constants.h"

ESP8266WiFiMulti WiFiMulti;

#define DHTPIN 4     // what digital pin we're connected to (GPIO4 in NodeMCU is D2)

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

static const int INTERVAL_BETWEEN_MEASUREMENTS = 1000 * 60; // 60 seconds
char url[200];

void setup() {

    USE_SERIAL.begin(115200);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFiMulti.addAP(Constants::WIFI_SSID, Constants::WIFI_PASSWORD);

}

void loop() {
  
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        // Reading temperature or humidity takes about 250 milliseconds!
        // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
        float h = dht.readHumidity();
        // Read temperature as Celsius (the default)
        float t = dht.readTemperature();

        // Check if any reads failed and exit early (to try again).
        if (isnan(h) || isnan(t)) {
          Serial.println("Failed to read from DHT sensor!");
          return;
        }

        float hic = dht.computeHeatIndex(t, h, false);

        USE_SERIAL.print("[HTTP] begin...\n");

        int h_hundreded = int(h*100.0);
        int t_hundreded = int(t*100.0);
        int hic_hundreded = int(hic*100.0);
        
        sprintf(url,
                "%s/log.php?logger_name=%s&log_time=%d&humidity=%d.%d&temperature_celsius=%d.%d&heat_index_celsius=%d.%d",
                Constants::LOG_SERVER_BASE_URL,
                Constants::LOGGER_NAME,
                time(NULL),
                h_hundreded / 100, h_hundreded % 100,       // A shitty workaround which compensates for the fact that Arduino IDE does not support float formatting.
                t_hundreded / 100, t_hundreded % 100,       // Also here.
                hic_hundreded / 100, hic_hundreded % 100);  // And here.

        USE_SERIAL.println(url);
        http.begin(url); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    
    // Wait a few seconds between measurements.
    delay(INTERVAL_BETWEEN_MEASUREMENTS);
}
