#include <Arduino.h>
#include <FastLED.h>

// #include <ESP8266WebServer.h>

#include <ArduinoOTA.h>

#include "BeatInfo.h"
#include "settings.h"

#include "mqtt.h"
#include "configuration.h"

#include "effects.h"

// #include "artnet.h"

#include "e131sync.h"

#include <HttpsWebServer.h>

CRGBArray<NUM_LEDS> leds;

BeatInfo beatInfo;

HttpsWebServer https;

void setup()
{

  Serial.begin(115200);

  config.setupWifiPortal("ProjektionFX");

  ArduinoOTA.begin();

  setupMqtt(config.getMQTTHost());

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, LED_MAX_MILLIAMP);

  webServer.begin();
  https.setupDNS();
  https.generateSSLCert();
  https.start();

#ifdef ARTNET_ENABLED
  e131sync.setup();
#endif
}

void loop()
{
  config.connectionGuard();
  config.webplayer();
  loopMqtt();
  ArduinoOTA.handle();

  beatInfo.loop();

  https.loop();

  //effectsRunner.run();

  FastLED.show();

#ifdef ARTNET_ENABLED
  e131sync.loop();
#endif

  EVERY_N_SECONDS(30)
  {
    effectsRunner.nextEffect();
  };

  // debug only :)
  // EVERY_N_SECONDS(10)
  // {
  //   beatInfo.changeBPM();
  // };
}