/**
 * Arduino Tools values
 * ------------
 * Board: NodeMC U1.0 (ESP-12E Module)
 * Flash size: 4M (3M SPIFFS)
 * IwIP Variant: v2 Lower Memory
 * VTables: Flash
 * CPU Frequency: 80MHz
 * Upload speed: 115200
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

// Add your own values here -------------------------
const char* serverURL = "http://192.168.11.7:4001";
const char* APID = "Buffalo-G-6280";
const char* APPassword = "abcdefghi12345";
// Time before additional button presses register
int LEDOnDuration = 5000; // milliseconds
// --------------------------------------------------

// For the sake of brevity, in case your ESP8266 is labeled differently
// static const uint8_t D0   = 16;
// static const uint8_t D1   = 5;
// static const uint8_t D2   = 4;
// static const uint8_t D3   = 0;
// static const uint8_t D4   = 2;
// static const uint8_t D5   = 14;
// static const uint8_t D6   = 12;
// static const uint8_t D7   = 13;
// static const uint8_t D8   = 15;
// static const uint8_t D9   = 3;
// static const uint8_t D10  = 1;

ESP8266WiFiMulti WiFiMulti;
bool running;
HTTPClient http;
int httpCode;


void setup() { 
  USE_SERIAL.begin(115200);
  // USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(APID, APPassword);

  running = false;

  // connect wires from button to ground and D2, use a 1k resistor
  pinMode(D2, INPUT_PULLUP);
  pinMode(D1, OUTPUT);
}

int buttonTriggered() {
    USE_SERIAL.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    http.begin(serverURL);

    httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

      // If server URL returns a response
      if (httpCode == HTTP_CODE_OK) {
        USE_SERIAL.println(http.getString());
      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    USE_SERIAL.print("Listening for button press...\n");

    if(digitalRead(D2) == LOW)
    {
      if(running == false)
      {
         running = true;
      }
    }

    if(running == true)
    {
      buttonTriggered();
      digitalWrite(D1, HIGH);
      // No additional button presses will register until delay() finishes
      delay(LEDOnDuration);
      running = false;
      digitalWrite(D1, LOW);
    }   
  }
}
