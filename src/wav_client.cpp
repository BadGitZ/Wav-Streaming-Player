
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

/* Set these to your desired credentials. */
const char *ssid = "Stefansinkt";
const char *password = "Geheim1!";

//Web/Server address to read/write from
const char *host = "192.168.137.11";

int toggle = 0;
int command = 0;

//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {
  pinMode(0, OUTPUT);
  delay(1000);
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {

  HTTPClient http;    //Declare object of class HTTPClient
  String Link;

  Link = "http://192.168.137.11:8080/test.html";

//  if (Serial.available() > 0) {

//    command = Serial.read();
//    Serial.println(command);
//    if (command == 53) {

      http.begin(Link);     //Specify request destination
      int httpCode = http.GET();            //Send the request
      String payload = http.getString();

      Serial.println(httpCode);   //Print HTTP return code

      if (httpCode != 200)
        Serial.println("Server Connection Error");
        Serial.println(payload);

        //Close connection
       http.end();
//    }
//  }


  if (toggle)
    digitalWrite(0, HIGH);
  else
    digitalWrite(0, LOW);
  toggle ^= 1;
}
//=======================================================================
