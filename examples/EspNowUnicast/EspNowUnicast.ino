#include <ESP8266WiFi.h>
#include <WifiEspNow.h>

static uint8_t PEER[] {0x5E, 0xCF, 0x7F, 0x90, 0xFA, 0xE8};

void printReceivedMessage(const uint8_t mac[6], const uint8_t* buf, size_t count, void* cbarg) {
  Serial.printf("Message from %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  for (int i = 0; i < count; ++i) {
    Serial.print(static_cast<char>(buf[i]));
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.print("MAC address of this node is ");
  Serial.println(WiFi.softAPmacAddress());

  WiFi.persistent(false);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("ESPNOW", "ESPNOW", 4);
  WiFi.softAPdisconnect(false);

  bool ok = WifiEspNow.begin();
  if (!ok) {
    Serial.println("WifiEspNow.begin() failed");
    ESP.restart();
  }

  WifiEspNow.onReceive(printReceivedMessage, nullptr);

  ok = WifiEspNow.addPeer(PEER);
  if (!ok) {
    Serial.println("WifiEspNow.addPeer() failed");
    ESP.restart();
  }
}

void loop() {
  char msg[60];
  int len = snprintf(msg, sizeof(msg), "hello ESP-NOW from %s at %lu", WiFi.softAPmacAddress().c_str(), millis());
  WifiEspNow.send(PEER, reinterpret_cast<const uint8_t*>(msg), len);
  delay(1000);
}