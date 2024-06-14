#include "DHT.h"                  // Include the DHT sensor library
#include <ESP8266WiFi.h>          // Include the ESP8266 WiFi library
#include <WiFiClient.h>           // Include the WiFi client library

#define DHTPIN D4                 // Define the pin connected to the DHT sensor
#define DHTTYPE DHT11             // Define the type of DHT sensor (DHT11)
#define LDRPIN A0                 // Define the analog pin for the LDR sensor

DHT dht(DHTPIN, DHTTYPE);         // Initialize the DHT sensor

float humidityData;               // Variable to store humidity data
float temperatureData;            // Variable to store temperature data
int lightIntensityData;           // Variable to store light intensity data

const char* ssid = "MIZ";         // WiFi SSID (name of the WiFi network)
const char* password = "iman/2002";  // WiFi password
char server[] = "192.168.0.106";  // IP address of the local server

WiFiClient client;                // Create a WiFi client object

void setup() {
  Serial.begin(115200);           // Initialize serial communication at 115200 baud
  delay(10);                      // Delay for stability
  dht.begin();                    // Start the DHT sensor
  pinMode(LDRPIN, INPUT);         // Set the LDR pin as input

  Serial.println();               
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);     // Connect to the WiFi network

  while (WiFi.status() != WL_CONNECTED) {  // Wait until the ESP is connected to WiFi
    delay(500);                  // Delay for half a second
    Serial.print(".");           // Print dots to indicate connection attempt
  }
  Serial.println("");
  Serial.println("WiFi connected");       // Print when WiFi is connected
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());         // Print the IP address assigned to the ESP
}

void loop() {
  humidityData = dht.readHumidity();       // Read humidity from the DHT sensor
  temperatureData = dht.readTemperature(); // Read temperature from the DHT sensor
  lightIntensityData = analogRead(LDRPIN); // Read light intensity from the LDR sensor

  // Print sensor readings to Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(humidityData);
  Serial.print(" %\t");  // Print humidity data and a tab character
  Serial.print("Temperature: ");
  Serial.print(temperatureData);
  Serial.print(" °C\t");  // Print temperature data and a tab character
  Serial.print("Light Intensity: ");
  Serial.println(lightIntensityData);  // Print light intensity data and end with a newline

  // Proceed to send data to PHP script
  Sending_To_phpmyadmindatabase();     // Call the function to send data to the database

  delay(30000);  // Interval: 30 seconds
}

void Sending_To_phpmyadmindatabase() {
  if (client.connect(server, 80)) {   // Connect to the server on port 80
    Serial.println("Connected to server");

    // Formulate the HTTP GET request:
    String url = "/env_monitor/execute_query.php"; // PHP script to handle data insertion
    url += "?humidity=";    // Add humidity parameter to the URL
    url += humidityData;    // Append humidity data to the URL
    url += "&temperature="; // Add temperature parameter to the URL
    url += temperatureData; // Append temperature data to the URL
    url += "&light_intensity="; // Add light intensity parameter to the URL
    url += lightIntensityData;  // Append light intensity data to the URL

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // Send HTTP GET request to the server:
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n"); // Send the request to the server

  } else {
    Serial.println("Connection failed"); // Print if the connection to the server fails
  }
}
