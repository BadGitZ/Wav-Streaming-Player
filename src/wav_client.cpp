
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

#define FILEPIN 2
#define HELLOPIN 16
#define FILECOUNT 1

HTTPClient http;    //Declare object of class HTTPClient

/* Set these to your desired credentials. */
const char *ssid = "GitzNet";
const char *password = "MoIstNeFagit666";

//Web/Server address to read/write from
const String host = "192.168.43.161:8080";

const String linkBase = "http://" + host;

int toggle = 0;

//=======================================================================
//                    Power On Setup
//=======================================================================

void setup() {

  pinMode(0, OUTPUT);
  pinMode(FILEPIN, INPUT);
  pinMode(HELLOPIN, INPUT);

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
//                    HTTP Request Function
//=======================================================================

String HttpGET(String Link) {

  http.begin(Link);     //Specify request destination
  int httpCode = http.GET();            //Send the request
  String payload = http.getString();

  Serial.println(httpCode);   //Print HTTP return code

  if (httpCode != 200)
    Serial.println("Server Connection Error");

  Serial.println(payload);

       //Close connection
  http.end();

  return payload;
}

//=======================================================================
//                    Serial Commands And Files Function
//=======================================================================

String SerialCommands() {
  if (Serial.available() > 0) {
    String command;
    command = Serial.read();
    Serial.println(command);
    return command;
  }
  return "0";
}

//=======================================================================
//                    Get File Names Function
//=======================================================================

void getFiles() {

    String Link;

    Link = linkBase + "/soundfiles";
    String filePayload = HttpGET(Link);
    String Files;
    int indexWav = 0;
    for (int i{0}; i <= (filePayload[FILECOUNT]-1); i++) {
      int index = filePayload.indexOf(">", indexWav);
      indexWav = filePayload.indexOf(".wav", index);
      for (int j{index+1}; j < indexWav; j++) {
        Files += filePayload[j];
      }
      Files += ",";
    }

    Serial.println(Files);
}

//=======================================================================
//                    Main Program Loop
//=======================================================================

void loop() {

  //Link = "http://192.168.43.161:8080/data/evGLMKfu";
  //Link = "http://192.168.43.161:8080/data/sXGXnJ6f";

  if (digitalRead(FILEPIN)) {
    getFiles();
  }

  else {
    Serial.println("nothing done");
  }
//    }
//  }


  if (toggle)
    digitalWrite(0, HIGH);
  else
    digitalWrite(0, LOW);
  toggle ^= 1;
}
//=======================================================================
