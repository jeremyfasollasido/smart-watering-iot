#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <WiFi.h> // For WiFi connection
#include <PubSubClient.h> // For MQTT client
#include <ArduinoJson.h> // For parsing JSON from UI commands (optional but good practice)

#define sensor 33
#define relay 4

// Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

// WiFi credentials
char ssid[] = ""; // CHANGE THIS
char pass[] = ""; // CHANGE THIS

// MQTT Broker details
const char* mqtt_server = "broker.hivemq.com"; // Public MQTT broker (for testing)
const int mqtt_port = 1883; // Default MQTT port
const char* mqtt_client_id = "ESP32_SmartWatering_Client";

// MQTT Topics
const char* MQTT_TOPIC_MOISTURE = "smartwatering/moisture";
const char* MQTT_TOPIC_MOTOR_CONTROL = "smartwatering/motor/control"; // UI publishes to this
const char* MQTT_TOPIC_MOTOR_STATUS = "smartwatering/motor/status"; // ESP32 publishes motor state

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Callback function for incoming MQTT messages (from UI)
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  // Check if the message is for motor control
  if (String(topic) == MQTT_TOPIC_MOTOR_CONTROL) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    String action = doc["action"];
    if (action == "on") {
      digitalWrite(relay, LOW); // Motor ON
      lcd.setCursor(0, 1);
      lcd.print("Motor is ON ");
      Serial.println("Motor turned ON");
      client.publish(MQTT_TOPIC_MOTOR_STATUS, "{\"status\":\"on\"}"); // Publish current motor status
    } else if (action == "off") {
      digitalWrite(relay, HIGH); // Motor OFF
      lcd.setCursor(0, 1);
      lcd.print("Motor is OFF");
      Serial.println("Motor turned OFF");
      client.publish(MQTT_TOPIC_MOTOR_STATUS, "{\"status\":\"off\"}"); // Publish current motor status
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_client_id)) {
      Serial.println("connected");
      // Once connected, subscribe to the motor control topic
      client.subscribe(MQTT_TOPIC_MOTOR_CONTROL);
      Serial.print("Subscribed to: ");
      Serial.println(MQTT_TOPIC_MOTOR_CONTROL);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  lcd.init();
  lcd.backlight();
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH); // Ensure relay is OFF initially

  lcd.setCursor(1, 0);
  lcd.print("System Loading");
  for (int a = 0; a <= 15; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(200);
  }
  lcd.clear();
}

// Get the soil moisture sensor values and publish
void publishSoilMoisture() {
  value = analogRead(sensor);
  value = map(value, 0, 4095, 0, 100);
  value = (value - 100) * -1; // Invert logic for better moisture reading (higher value = more moisture)

  // Construct JSON payload for moisture
  StaticJsonDocument<100> doc;
  doc["moisture"] = value;
  char jsonBuffer[100];
  serializeJson(doc, jsonBuffer);

  client.publish(MQTT_TOPIC_MOISTURE, jsonBuffer);
  Serial.print("Published moisture: ");
  Serial.println(jsonBuffer);

  lcd.setCursor(0, 0);
  lcd.print("Moisture :");
  lcd.print(value);
  lcd.print("%   "); // Added % for clarity
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Must be called frequently to process incoming MQTT messages

  long now = millis();
  if (now - lastMsg > 5000) { // Publish moisture every 5 seconds
    lastMsg = now;
    publishSoilMoisture();
  }
  delay(10); // Small delay to avoid watchdog issues
}
