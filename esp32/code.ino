#include "aws_secrets.h"

#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define SENSOR_TEMPERATURE_PIN 26
#define SENSOR_HUMIDITY_ONE_PIN 33
#define SENSOR_HUMIDITY_TWO_PIN 32

#define DATA_SEND_INTERVAL_MS 1000 * 60 * 60 // 1 hour 

#define AWS_IOT_DATA_TOPIC "iot/esp32/garden/monitoring"

WiFiClientSecure net;
PubSubClient client(net);

OneWire oneWire(SENSOR_TEMPERATURE_PIN);
DallasTemperature sensors(&oneWire);

const char* ssid = "<CHANGE_HERE>";
const char* password = "<CHANGE_HERE>";
const char* ntpServer = "pool.ntp.org";

float celsius = 0.0;
long sensorHumidityOnePercentage = 0;
long sensorHumidityTwoPercentage = 0;
unsigned long epochTime = 0;

unsigned long getCurrentTimestamp();
void establishAWSSecureConnection();
void publishSensorDataToAWS();

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  Serial.println("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println(ssid);

  configTime(0, 0, ntpServer);
  
  establishAWSSecureConnection();
  
  sensors.begin();
}

void loop() {
  epochTime = getCurrentTimestamp();
  sensors.requestTemperatures();

  celsius = sensors.getTempCByIndex(0);
  sensorHumidityOnePercentage = map(analogRead(SENSOR_HUMIDITY_ONE_PIN), 0, 4095, 100, 0);
  sensorHumidityTwoPercentage = map(analogRead(SENSOR_HUMIDITY_TWO_PIN), 0, 4095, 100, 0);

  publishSensorDataToAWS();
  client.loop();

  delay(DATA_SEND_INTERVAL_MS);
}

unsigned long getCurrentTimestamp() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return 0;
  }
  time(&now);
  return now;
}

void establishAWSSecureConnection() {
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);

  client.setServer(AWS_IOT_ENDPOINT, 8883);

  Serial.print("Connecting to AWS...");

  while (!client.connect(AWS_THING_NAME)) {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected()) {
    Serial.println("AWS IoT Timeout!");
    return;
  }

  Serial.println("CONNECTED");
}

void publishSensorDataToAWS() {
  if (epochTime == 0) {
    Serial.println("Timestamp not ready. Skipping publish.");
    return;
  }

  StaticJsonDocument<400> jsonDocument;
  JsonArray dataArray = jsonDocument.createNestedArray("data");

  // Garden 1
  JsonObject garden1 = dataArray.createNestedObject();
  garden1["id"] = "garden_1";
  garden1["timestamp"] = epochTime;
  garden1["temperature"] = celsius;
  garden1["humidity"] = sensorHumidityOnePercentage;

  // Garden 2
  JsonObject garden2 = dataArray.createNestedObject();
  garden2["id"] = "garden_2";
  garden2["timestamp"] = epochTime;
  garden2["temperature"] = celsius;
  garden2["humidity"] = sensorHumidityTwoPercentage;

  String jsonMessage;
  serializeJson(jsonDocument, jsonMessage);

  if (!client.connected()) {
    establishAWSSecureConnection();  // Reconnect if disconnected
  }

  client.publish(AWS_IOT_DATA_TOPIC, jsonMessage.c_str());
  Serial.println("Message published to AWS");
}
