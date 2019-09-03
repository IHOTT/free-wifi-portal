#include <WiFi.h>
#include <DNSServer.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
WiFiServer server(80);

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>Time Colony Wifi</title></head><body>"
  "</br></br></br>"
  "<div style=\"text-align: center;\">"
  "┍━━━━━━*⑅*❀⑅*❀⑅*❀⑅*❀⑅*❀⑅*━━━━━━┑"
  "<h1>Fuck your Instagram!</h1>"
  "┕━━━━━━*⑅*❀⑅*❀⑅*❀⑅*❀⑅*❀⑅*━━━━━━┙"
  "</br></br></br>"
  "<h2>Sparkle Ponies Fooled: 11</h2>"
  "</div>"
  "</body></html>";

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Time Colony ♥ Free Wifi ♥");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(responseHTML);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}
