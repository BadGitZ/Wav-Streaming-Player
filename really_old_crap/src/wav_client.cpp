
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#define FILEPIN 2
#define HELLOPIN 16
#define FILECOUNT 0

HTTPClient http;    //Declare object of class HTTPClient

/* Set these to your desired credentials. */
const char *ssid = "GitzNet";
const char *password = "MoIstNeFagit666";

//Web/Server address to read/write from
const String host = "192.168.43.161:8080";

const String linkBase = "http://" + host;
String Link;

String filePayload;

int toggle = 0;


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

       //Close connection
  //http.end();

  return payload;
}

//=======================================================================
//                    Serial Commands And Files Function
//=======================================================================

String serialCommands() {
  if (Serial.available() > 0) {
    String command;
    command = Serial.readString();
    Serial.println(command);
    return command;
  }
  return "0";
}



void getAudio() {

}


//=======================================================================
//                    Get File Names Function
//=======================================================================

String getFiles() {

    Link = linkBase + "/soundfiles";
    filePayload = HttpGET(Link);
    //char files[filePayload[FILECOUNT]];
    String files;
    int indexWav = 0;
    int fileCount = filePayload[FILECOUNT] - '0';
    Serial.println(fileCount);
    for (int i = 0; i <= (fileCount-1); i++) {
      int index = filePayload.indexOf(">", indexWav);
      indexWav = filePayload.indexOf(".wav", index);
      for (int j{index+1}; j < indexWav; j++) {
        files += filePayload[j];
      }
      files += ",";
    }
    Serial.println(files);
    return filePayload;
}



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

  filePayload = getFiles();
}



//=======================================================================
//                    Main Program Loop
//=======================================================================

void loop() {

  //Link = "http://192.168.43.161:8080/data/evGLMKfu";
  //Link = "http://192.168.43.161:8080/data/sXGXnJ6f";

  String command = serialCommands();

  if (command.endsWith(".wav")) {
        int fileIndex = filePayload.indexOf(command);
        int urlFirstIndex = filePayload.indexOf("[", fileIndex);
        int urlLastIndex = filePayload.indexOf("]", fileIndex);
        String wantedUrl;
        for (int i = urlFirstIndex+1; i < urlLastIndex; i++) {
          wantedUrl += filePayload[i];
        }
        String fileInfo;
        fileInfo = HttpGET(linkBase + "/info" + wantedUrl);
        Serial.println(fileInfo);
        Serial.println(ESP.getFreeHeap());

        for (int i = 0; i<= 5; i++) {
          String miau = HttpGET(linkBase + "/data" + wantedUrl);
          Serial.println(ESP.getFreeHeap());
        }
  }
  else if (command == "0")  {

  }
  else {
    Serial.println("unknown command");
  }

  if (toggle)
    digitalWrite(0, HIGH);
  else
    digitalWrite(0, LOW);
  toggle ^= 1;

}
//=======================================================================
