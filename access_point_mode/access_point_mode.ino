#include <ESP8266WiFi.h>
const char *ssid = "NtD";
const char *password = "12345678";
IPAddress local_IP(192,168,4,21);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);
void setup() {
  // put your setup code here, to run once:
  Serial.begin (115200);

  WiFi.softAPConfig(local_IP, gateway, subnet);
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.print(ESP.getChipId());
}
void loop() {
  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
}
