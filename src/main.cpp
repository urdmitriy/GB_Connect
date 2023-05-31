#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define LED 25

const char wifi_ssid[] = {"MTS_GPON_3BF6"};
const char wifi_pass[] = {"CdARCCTH"};

WiFiServer server(80);

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
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
  
  digitalWrite(LED, 1);
  delay(1000);
  digitalWrite(LED, 0);

  server.begin();
}

int value = 0;

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:

            client.print("<!DOCTYPE html><html lang=\"ru\"><head><meta charset=\"UTF-8\"><title>Управление светодиодом</title></head>");

            client.print("<style>body{width: 500px;margin: 0px auto;background: antiquewhite;text-align: center;}a{text-decoration: none;color: black;border: 1px solid black;border-radius: 5px;background: darkgoldenrod;margin: 5px auto;padding: 5px;width: 100%;}</style>");

            client.print("<body><div style=\"display: flex; margin: 5px auto; width: 500px\">");

            if (digitalRead(LED)){
              client.print("<div style=\"width: 50px; height: 50px; border: 1px solid black; background: green\"></div>");
            }
            else
            {
              client.print("<div style=\"width: 50px; height: 50px; border: 1px solid black; background: white\"></div>");
            }
            
            client.print("<p style=\"padding-left: 10px\">Состояние светодиода</p></div>");

            client.print("<div style=\"display:flex; flex-direction: column; width: 500px\"><a href=\"/H\">Вкл</a><a href=\"/L\">Выкл</a></div></body>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}