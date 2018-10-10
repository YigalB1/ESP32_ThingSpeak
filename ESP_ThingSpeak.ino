// from: https://www.mathworks.com/help/thingspeak/measure-arduino-wifi-signal-strength-with-esp32.html
// www.thingspeak.com
// Channel name: ESP32 Signal Strength
// Channel ID: 599443
// API key: X35I1D69XR9HA7K9



#include <WiFi.h>

#define buttonPin 0
#define LEDPin 2

// Network information
char* ssid = "WIFINAME";
const char* password = "PPPPPPPPP";

// ThingSpeak Settings
char server[] = "api.thingspeak.com";
String writeAPIKey = "XXXXXXXXXXXXXXXX";

// Constants
const unsigned long postingInterval = 15L * 1000L;

// Global Variables
unsigned long lastConnectionTime = 0;
int measurementNumber = 0;


void setup(){

    Serial.begin(115200);
    pinMode(buttonPin,INPUT);
    pinMode(LEDPin, OUTPUT);
    connectWiFi();
}


void loop(){
const int numberPoints = 7;
float wifiStrength;

  // In each loop, make sure there is always an internet connection.
    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
    }

    // If  a button press is detected, write the data to ThingSpeak.
    if (digitalRead(buttonPin) == LOW){
        if (millis() - lastConnectionTime > postingInterval) {
            blinkX(2,250); // Verify the button press.
            wifiStrength = getStrength(numberPoints);
            httpRequest(wifiStrength, measurementNumber);
            blinkX(measurementNumber,200);  // Verify that the httpRequest is complete.
            measurementNumber++;
        }

    }
}
// Connect your device to the wireless network using the connectWiFi function. The device blinks five times fast once connected.

void connectWiFi(){

    while (WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password);
        delay(3000);
    }

    // Show the user a connection is successful.
    Serial.println("Connected");
    blinkX(5,50);
}
// Connect to the ThingSpeak server and build the data strings for the HTTP POST command using the httpRequest function.

void httpRequest(float field1Data, int field2Data) {

    WiFiClient client;

    if (!client.connect(server, 80)){

        Serial.println("connection failed");
        lastConnectionTime = millis();
        client.stop();
        return;
    }

    else{

        // create data string to send to ThingSpeak
        String data = "field1=" + String(field1Data) + "&field2=" + String(field2Data); //shows how to include additional field data in http post

        // POST data to ThingSpeak
        if (client.connect(server, 80)) {

            client.println("POST /update HTTP/1.1");
            client.println("Host: api.thingspeak.com");
            client.println("Connection: close");
            client.println("User-Agent: ESP32WiFi/1.1");
            client.println("X-THINGSPEAKAPIKEY: "+writeAPIKey);
            client.println("Content-Type: application/x-www-form-urlencoded");
            client.print("Content-Length: ");
            client.print(data.length());
            client.print("\n\n");
            client.print(data);

            Serial.println("RSSI = " + String(field1Data));
            lastConnectionTime = millis();
        }
    }
    client.stop();
}
// Take several measurements and return the average value to the main loop with getStrength.

// Take a number of measurements of the WiFi strength and return the average result.
int getStrength(int points){
    long rssi = 0;
    long averageRSSI=0;

    for (int i=0;i < points;i++){
        rssi += WiFi.RSSI();
        delay(20);
    }

   averageRSSI=rssi/points;
    return averageRSSI;
}
// Finally, use the blinkX function to flash the device LED. The flashes allow the board to communicate with you when not connected to the computer via USB.

// Flash the LED a variable number of times with a variable delay.
void blinkX(int numTimes, int delayTime){
    for (int g=0;g < numTimes;g++){

        // Turn the LED on and wait.
        digitalWrite(LEDPin, HIGH);
        delay(delayTime);

        // Turn the LED off and wait.
        digitalWrite(LEDPin, LOW);
        delay(delayTime);

    }
}
