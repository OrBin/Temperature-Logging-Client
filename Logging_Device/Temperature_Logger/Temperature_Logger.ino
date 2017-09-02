/**
 *                               Temperature Logger for NodeMCU
 * 
 * -----------------------------------------------------------------------------------------
 * 
 * This code tests the temperature, humidity and heat index once a minute using DHT11 sensor,
 * and send it to a server to save the data for future use.
 * 
 * This code is based on BasicHTTPClient.ino example which comes with NodeMCU library, 
 * and DHTtester example which comes with DHT sensor library.
 * 
 * -----------------------------------------------------------------------------------------
 * Used libraries:
 * adafruit/DHT-sensor-library v1.2.3
 * https://github.com/adafruit/DHT-sensor-library/
 * 
 * bblanchon/ArduinoJson v5.6.7
 * https://github.com/bblanchon/ArduinoJson/
 */
 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"
#include <ArduinoJson.h>

#include "Config.h"

ESP8266WiFiMulti WiFiMulti;

#define DHTPIN 4     // what digital pin we're connected to (GPIO4 in NodeMCU is D2)

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
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


void setup() {

    Serial.begin(115200);
   // Serial.setDebugOutput(true);

    Serial.println();
    Serial.println();
    Serial.println();

    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }

    WiFiMulti.addAP(Config::WIFI_SSID, Config::WIFI_PASSWORD);

}

void loop() {
  
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED))
    {
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

        Serial.print("[HTTP] begin...\n");

        Serial.println(Config::LOGGING_URL);
        http.begin(Config::LOGGING_URL); //HTTP

        Serial.print("[HTTP] POST...\n");
        // start connection and send HTTP header
        http.addHeader("Content-Type", "application/json");
        
        StaticJsonBuffer<200> jsonBuffer;
        
        JsonObject& root = jsonBuffer.createObject();
        root["logger"] = Config::LOGGER_ID;
        root["humidity"] = h;
        root["temperature_celsius"] = t;
        root["heat_index_celsius"] = hic;

        int buffer_size = root.measureLength() + 1;

        char buffer[buffer_size];
        root.printTo(buffer, buffer_size);
        root.printTo(Serial);
        Serial.println();

        Serial.println(Config::LOGGING_URL);
        http.begin(Config::LOGGING_URL); //HTTP

        Serial.print("[HTTP] POST...\n");
        // start connection and send HTTP header
        http.addHeader("Content-Type", "application/json");

        int httpCode = http.POST((char*)buffer);

        http.writeToStream(&Serial);
        Serial.println();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] POST... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();

        
        // Wait between measurements.
        delay(Config::MILLIS_BETWEEN_MEASUREMENTS);
    }
    else
    {
      Serial.println("No wifi connection");
    }
}
