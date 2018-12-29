/*
  Air Monitor
  by Sasha Magee

 This sketch connects to a website (http://www.google.com)
 using a WiFi shield.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
*/

#include <SPI.h>
#include <WiFi101.h>
#include <string.h>
#include <Adafruit_NeoPixel.h>
#include "arduino_secrets.h" 

#define PIN 6

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
char server[] = "www.airnowapi.org";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

//Define the NeoPixel(s)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN);

void setup() {
  pinMode(2, OUTPUT);

  // for pixel
  strip.begin();
  strip.setBrightness(50);
  strip.show(); // Initialize all pixels to 'off'
  
  for (int j = 0; j < 3; j++) {
    digitalWrite(2, LOW);
    delay(100);
    digitalWrite(2, HIGH);
    delay(100);
  }
  digitalWrite(2, LOW);
  
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Starting");

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println(ERROR_NOT_PRESENT);
    // don't continue:
    while (true);
  }
  Serial.print("Attempting SSID: ");
  Serial.println(ssid);
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    
  status = WiFi.begin(ssid, pass);

  // attempt to connect to WiFi network:
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      digitalWrite(2, LOW);
      delay(100);
      digitalWrite(2, HIGH);
      delay(100);
  }

  Serial.println();
  Serial.println("Connected");
  printWiFiStatus();

  Serial.println("\nStarting connex");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connex");
    // Make a HTTP request:
    client.println(AIRNOW_API_CALL);
    client.println(AIRNOW_HOST);
    client.println("Cnx: close");
    client.println();
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  int linecount = 0;
  int fieldcount = 0;
  while (client.available()) {
    char c = client.read();
    if (linecount == 2) {
      if (c == ',') {
        fieldcount++;
      } else if (fieldcount == 9) {
        if (c != '\"') {
          //making assumption here that we have only one digit
          Serial.write(c);
          c-= 48;
          displayValue(c);
        }
      }
    }
    if (c == '\n') {
      linecount++;
    }
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("discon");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
}

//currently just prints the color corresponding to airquality
void displayValue(int val) {
  char color[7];
  uint32_t colors[5] = { 
    strip.Color(0x00, 0xff, 0x00), //green
    strip.Color(0xff, 0xFF, 0x55), //yellow
    strip.Color(0xef, 0x85, 0x33), //orange
    strip.Color(0xea, 0x00, 0x00), //red
    strip.Color(0x8c, 0x1a, 0x4b) //purple
  };
  char* str = "green yelloworangered   purple";
  strncpy(color, str + ((val -1) *6), 6);
  color[6] = '\0';
  Serial.println(color);
  for (int i = 0; i < 8; i++) {
    strip.setPixelColor(i, colors[val -1]);
  }
  strip.show();
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
