#include <Arduino.h>
#include <WiFi.h>

#include "websocket.h"
#include "storage.h"
#include "mode/mode.h"
#include "secrets.h"
#include "ota.h"
#include "webserver.h"

#include "accelerometer.h"

#ifdef Accelerometer
Accelerometer accelerometer;
#endif

void setup()
{
  Serial.begin(115200);

  pinMode(PIN_LATCH, OUTPUT);
  pinMode(PIN_CLOCK, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);
  pinMode(PIN_ENABLE, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  clearScreenAndBuffer(renderBuffer);

  // https://randomnerdtutorials.com/esp32-save-data-permanently-preferences/
  storage.begin("led-wall", false);
  storage.getBytes("data", renderBuffer, sizeof(renderBuffer));
  storage.end();
  renderScreen(renderBuffer);

  // wifi
  int attempts = 0;
  WiFi.setHostname(WIFI_HOSTNAME);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED && attempts < 7)
  {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Resetting");
    ESP.restart();
  }
  else
  {
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
  }

  // server
  initOTA(server);
  initWebsocketServer(server);
  initWebServer();

  // uncomment to figure out the initial parameters
  #ifdef Accelerometer
    // accelerometer.startCalibration();
    accelerometer.setup();
  #endif
}

void loop()
{
  #ifdef Accelerometer
    accelerometer.loop();
  #endif
  modeLoop();
  cleanUpClients();
}
