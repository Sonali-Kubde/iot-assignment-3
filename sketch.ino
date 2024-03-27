#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "aws_credentials.h" // Make sure this file contains all your AWS credentials
#include <ArduinoJson.h>

#define SENSOR_PORT 14
#define SENSOR_KIND DHT11

#define MQTT_TOPIC_OUTGOING "sensor_esp/pub"
#define MQTT_TOPIC_INCOMING "sensor_esp/sub"

DHT climateSensor(SENSOR_PORT, SENSOR_KIND);

WiFiClientSecure encryptedClient;
PubSubClient mqttBroker(encryptedClient);

float airHumidity, airTemperature, co2Concentration;

void InitializeConnection() {
  Serial.print("Connecting to WiFi Network: ");
  Serial.println(ROUTER_USERNAME);

  WiFi.begin(ROUTER_USERNAME, ROUTER_SECRET);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  encryptedClient.setCACert(AMAZON_TOP_DOMAIN);
  encryptedClient.setCertificate(AMAZON_DC);
  encryptedClient.setPrivateKey(AMAZON_PP);

  mqttBroker.setServer(DEVICE_DATA, 8883);
  mqttBroker.setCallback(IncomingMessageHandler);

  ConnectToBroker();
}

void ConnectToBroker() {
  Serial.println("Connecting to MQTT Broker...");
  while (!mqttBroker.connected()) {
    if (mqttBroker.connect(SENSOR_THING)) {
      mqttBroker.subscribe(MQTT_TOPIC_INCOMING);
      Serial.println("Connected to MQTT Broker.");
    } else {
      Serial.print("Connection to MQTT Broker failed, rc=");
      Serial.println(mqttBroker.state());
      Serial.println("Trying again in 5 seconds.");
      delay(5000);
    }
  }
}

void TransmitSensorData() {
  StaticJsonDocument<256> jsonDocument;
  jsonDocument["humidity"] = airHumidity;
  jsonDocument["temp"] = airTemperature;
  jsonDocument["co2_level"] = co2Concentration;
  char buffer[512];
  serializeJson(jsonDocument, buffer);

  if (!mqttBroker.publish(MQTT_TOPIC_OUTGOING, buffer)) {
    Serial.println("Failed to publish sensor data.");
  } else {
    Serial.println("Sensor data published.");
  }
}

void IncomingMessageHandler(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Give time for the serial monitor to initialize, especially on slower systems.
  climateSensor.begin();
  InitializeConnection();
}

void loop() {
  if (!mqttBroker.connected()) {
    ConnectToBroker();
  }

  airHumidity = climateSensor.readHumidity();
  airTemperature = climateSensor.readTemperature();
  co2Concentration = random(400, 5000); // Simulate CO2 reading

  if (!isnan(airHumidity) && !isnan(airTemperature)) {
    Serial.printf("Humidity: %.1f%% Temp: %.1f°C CO2: %.0fppm\n", airHumidity, airTemperature, co2Concentration);
    TransmitSensorData();
  } else {
    Serial.println("Sensor read error");
  }

  mqttBroker.loop();
  delay(2000); // Adjust based on how often you want to send data
}
