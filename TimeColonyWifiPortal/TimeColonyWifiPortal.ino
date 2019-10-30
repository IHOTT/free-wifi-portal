#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include "SPIFFS.h"
#include <Preferences.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"

DNSServer dnsServer;
AsyncWebServer server(80);

#define RESPONSE_DEBOUNCE_MILLIS 7000
long lastIncrement = millis();
Preferences preferences;

class CaptiveRequestHandler : public AsyncWebHandler {
  public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request) {
      //request->addInterestingHeader("ANY");
      return true;
    }

    void handleRequest(AsyncWebServerRequest *request) {
      if (request->url() == "/2d_time_waffle.png") {
        request->send(SPIFFS, "/2d_time_waffle.png");
      }
      else if (request->url() == "/isometric_time_waffle.png") {
        request->send(SPIFFS, "/isometric_time_waffle.png");
      }
      else {
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        String responseHTML = "<!DOCTYPE html><html><head><title>Time Colony Wifi</title><meta charset='utf-8'></head><body> <style>html, body {  width: 100%;    height: 100%;  -webkit-font-smoothing: antialiased;  display: flex;  justify-content: center;  align-items: center;} h1 span {  position: relative;  top: 20px;  display: inline-block;  animation: bounce .3s ease infinite alternate;  font-family: 'Titan One', cursive;  font-size: 120pt;  color: #7788ff;  text-shadow: 0 1px 0 #CCC,               0 2px 0 #CCC,               0 3px 0 #CCC,               0 4px 0 #CCC,               0 5px 0 #CCC,               0 6px 0 transparent,               0 7px 0 transparent,               0 8px 0 transparent,               0 9px 0 transparent,               0 10px 10px rgba(0, 0, 0, .4);} h1 span:nth-child(1) { animation-delay: .5s; } h1 span:nth-child(2) { animation-delay: 1.0s; } h1 span:nth-child(3) { animation-delay: 1.5s; } h1 span:nth-child(4) { animation-delay: 1.6s; } h1 span:nth-child(5) { animation-delay: 1.7s; } h1 span:nth-child(6) { animation-delay: 1.8s; } h1 span:nth-child(7) { animation-delay: 1.9s; } h1 span:nth-child(8) { animation-delay: 2.0s; } h1 span:nth-child(9) { animation-delay: 2.1s; } h1 span:nth-child(10) { animation-delay: 2.2s; } h1 span:nth-child(11) { animation-delay: 2.3s; } h1 span:nth-child(12) { animation-delay: 2.4s; } h1 span:nth-child(13) { animation-delay: 2.5s; } @keyframes bounce {  100% {    top: -20px;    text-shadow: 0 1px 0 #CCC,                 0 2px 0 #CCC,                 0 3px 0 #CCC,                 0 4px 0 #CCC,                 0 5px 0 #CCC,                 0 6px 0 #CCC,                 0 7px 0 #CCC,                 0 8px 0 #CCC,                 0 9px 0 #CCC,                 0 50px 25px rgba(0, 0, 0, .2);  }} </style> <div style='text-align: center;'> <h1>   <span style='color: #000000;'>Fuck&nbsp;</span>   <span style='color: #000000;'>Your</span>   </br>   <span style='color: hsl(0, 100%,50%);'>I</span>   <span style='color: hsl(36, 100%,50%);'>n</span>   <span style='color: hsl(55, 100%,50%);'>s</span>   <span style='color: hsl(72, 100%,50%);'>t</span>   <span style='color: hsl(144, 100%,50%);'>a</span>   <span style='color: hsl(180, 100%,50%);'>g</span>   <span style='color: hsl(216, 100%,50%);'>r</span>   <span style='color: hsl(252, 100%,50%);'>a</span>   <span style='color: hsl(288, 100%,50%);'>m</span>   <span style='color: hsl(324, 100%,50%);'>!</span> </h1>  </br></br></br> <img width=550px src='isometric_time_waffle.png' alt='Time Colony Waffles' /> </br></br></br>  <h2 style='font-size: 30pt'>Sparkle Ponies Bamboozled: $visitorCount$</br>❤️</h2></div></body></html>";

        // get old visitor count from NVS
        preferences.begin("my-app", false);
        int visitorCount = 0;
        unsigned int counter = preferences.getUInt("counter", 0);
        //  Serial.printf("Old visitor count: %u\n", counter);

        if (millis() - lastIncrement > RESPONSE_DEBOUNCE_MILLIS) {
          // increment visitor count
          counter++;
          // save the new visitor count to NVS
          preferences.putUInt("counter", counter);
          Serial.printf("New visitor count: %u\n", counter);
          preferences.end();
          
          lastIncrement = millis();
        }

        responseHTML.replace("$visitorCount$", String(counter));
        response->print(responseHTML);
        request->send(response);
      }
    }
};


void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // uncomment this to reset the counter at each boot
//  preferences.begin("my-app", false);
//  preferences.clear();
//  preferences.end();

  //your other setup stuff...
  WiFi.softAP("⏳Time Colony 🧇Free Wifi");
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP

  //  server.on("/2d_time_waffle.png", HTTP_GET, [](AsyncWebServerRequest *request){
  //    request->send(SPIFFS, "/2d_time_waffle.png");
  //  });
  //  server.on("/isometric_time_waffle.png", HTTP_GET, [](AsyncWebServerRequest *request){
  //    request->send(SPIFFS, "/isometric_time_waffle.png");
  //  });
  //more handlers...
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
}
