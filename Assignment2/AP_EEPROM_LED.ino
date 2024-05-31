#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// Pin definitions
const int outputPin = D1; // GPIO pin for the LED or Relay

// WiFi credentials
char ssid[32] = "";
char password[32] = "";

// Output status
bool outputStatus = false;

// EEPROM addresses
const int addrSSID = 0;
const int addrPassword = 32;
const int addrOutputStatus = 64;

// Web server
ESP8266WebServer server(80);

// Function declarations
void handleRoot();
void handleSave();
void setupAP();
void readEEPROM();
void writeEEPROM();
void connectToWiFi();

void setup() {
  Serial.begin(115200);
  pinMode(outputPin, OUTPUT);

  // Initialize EEPROM
  EEPROM.begin(512);
  readEEPROM();

  // Check if WiFi credentials are stored in EEPROM
  if (strlen(ssid) == 0) {
    setupAP(); // Start in AP mode if no WiFi credentials
  } else {
    connectToWiFi(); // Try to connect to WiFi
  }
}

void loop() {
  server.handleClient(); // Handle web server

  // Reconnect if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }
}

// Handle root URL request
void handleRoot() {
  String html = "<html><head>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background-color: #f4f4f4; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; }";
  html += ".container { width: 100%; max-width: 600px; padding: 20px; background-color: #fff; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }";
  html += "h2 { text-align: center; }";
  html += "input[type='text'], input[type='password'], select { width: 100%; padding: 10px; margin: 10px 0; border: 1px solid #ccc; border-radius: 5px; box-sizing: border-box; }";
  html += "input[type='submit'] { width: 100%; padding: 10px; background-color: #007BFF; border: none; color: #fff; border-radius: 5px; cursor: pointer; }";
  html += "input[type='submit']:hover { background-color: #0056b3; }";
  html += "@media (max-width: 600px) {";
  html += "  .container { padding: 10px; }";
  html += "  input[type='text'], input[type='password'], select, input[type='submit'] { padding: 8px; }";
  html += "}";
  html += "</style>";
  html += "<script>";
  html += "function confirmSubmit() {";
  html += "  return confirm('Are you sure you want to save the configuration?');";
  html += "}";
  html += "</script>";
  html += "</head><body>";
  html += "<div class='container'>";
  html += "<h2>Device Configuration</h2>";
  html += "<form action='/save' method='POST' onsubmit='return confirmSubmit();'>";
  html += "SSID: <input type='text' name='ssid' value='" + String(ssid) + "' required><br>";
  html += "Password: <input type='password' name='password' value='" + String(password) + "' required><br>";
  html += "Output Status: <select name='status'><option value='ON'" + String(outputStatus ? " selected" : "") + ">ON</option><option value='OFF'" + String(!outputStatus ? " selected" : "") + ">OFF</option></select><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form>";
  html += "</div>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Handle save configuration request
void handleSave() {
  // Save WiFi credentials and output status from form
  String newSSID = server.arg("ssid");
  String newPassword = server.arg("password");
  String newStatus = server.arg("status");

  newSSID.toCharArray(ssid, 32);
  newPassword.toCharArray(password, 32);
  outputStatus = (newStatus == "ON");

  // Write to EEPROM
  writeEEPROM();

  // Send response to client
  server.send(200, "text/plain", "Configuration saved. Rebooting...");

  // Delay before rebooting
  delay(1000);
  ESP.restart();
}

// Setup access point mode
void setupAP() {
  WiFi.softAP("LED_Config");

  // Configure server routes
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);

  // Start server
  server.begin();
  Serial.println("AP Mode: Configure device via 192.168.4.1");
}

// Read WiFi credentials and output status from EEPROM
void readEEPROM() {
  EEPROM.get(addrSSID, ssid);
  EEPROM.get(addrPassword, password);
  EEPROM.get(addrOutputStatus, outputStatus);

  // Validate EEPROM content
  if (ssid[0] == '\0' || password[0] == '\0') {
    ssid[0] = '\0';
    password[0] = '\0';
    outputStatus = false;
  }
}

// Write WiFi credentials and output status to EEPROM
void writeEEPROM() {
  EEPROM.put(addrSSID, ssid);
  EEPROM.put(addrPassword, password);
  EEPROM.put(addrOutputStatus, outputStatus);
  EEPROM.commit();
}

// Connect to WiFi
void connectToWiFi() {
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi...");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  // Check connection status
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    digitalWrite(outputPin, outputStatus ? HIGH : LOW);
  } else {
    Serial.println("Failed to connect. Starting AP mode.");
    setupAP();
  }
}
