#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char wifi_ssid[] = {"MTS_GPON_3BF6"};
const char wifi_pass[] = {"CdARCCTH"};
const char address_page[] = {"https://jsonplaceholder.typicode.com/posts/1"};

void setup() {
  Serial.begin(115200);
  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.print("Connecting");
  while (WiFi.status() != wl_status_t::WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Ok");
  Serial.println("Ip address: " + WiFi.localIP().toString());
  Serial.println("MAC address: " + WiFi.macAddress());

  HTTPClient client;
  client.begin(address_page);
  Serial.printf("Address page: %s\n\r", address_page);
  int response_code = client.GET();
  if (response_code == HTTP_CODE_OK){
    Serial.printf("Response code: %d\n\r", response_code);
    Serial.println("Data:");
    Serial.println(client.getString());
  }
  else{
    Serial.printf("Response code: %d\n\r", response_code);
  }
}

void loop() {
  
}