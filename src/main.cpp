#include <Adafruit_EMC2101.h>

Adafruit_EMC2101  emc2101;
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "secrets.h"
WiFiClient espClient;

IPAddress server(mqtt_server_ip);
PubSubClient client(server, 1883, espClient);
int setup_wifi() {
  int timeout = 0;
  String hostname = "espressiv_" + WiFi.macAddress();
  hostname.replace(":", "");
  Serial.println(hostname);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  Serial.println(WiFi.macAddress());
  WiFi.setHostname(hostname.c_str());
  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED and (timeout < 20)) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("RSSI:");
    Serial.println(WiFi.RSSI());
    return 0;
  } else {
    Serial.println("WiFi not connected");
    return -1;
  }
}

void setup(void) {
  Serial.begin(115200);
  Serial.println("Fan Controller");

  // Try to initialize!
  if (!emc2101.begin()) {
    Serial.println("Failed to find EMC2101 chip");
    while (1) { delay(10); }
  }
  Serial.println("EMC2101 Found!");

  //  emc2101.setDataRate(EMC2101_RATE_1_16_HZ);
  Serial.print("Data rate set to: ");
  switch (emc2101.getDataRate()) {
    case EMC2101_RATE_1_16_HZ: Serial.println("1/16_HZ"); break;
    case EMC2101_RATE_1_8_HZ: Serial.println("1/8_HZ"); break;
    case EMC2101_RATE_1_4_HZ: Serial.println("1/4_HZ"); break;
    case EMC2101_RATE_1_2_HZ: Serial.println("1/2_HZ"); break;
    case EMC2101_RATE_1_HZ: Serial.println("1 HZ"); break;
    case EMC2101_RATE_2_HZ: Serial.println("2 HZ"); break;
    case EMC2101_RATE_4_HZ: Serial.println("4 HZ"); break;
    case EMC2101_RATE_8_HZ: Serial.println("8 HZ"); break;
    case EMC2101_RATE_16_HZ: Serial.println("16 HZ"); break;
    case EMC2101_RATE_32_HZ: Serial.println("32 HZ"); break;
  }

  emc2101.enableTachInput(true);
  emc2101.setPWMDivisor(0);
  emc2101.setDutyCycle(50);
  if (setup_wifi() == 0) {
    // client.setCallback(callback);
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("fan_controller", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
    if (client.connected()) {
        Serial.println("Connected to MQTT");
    }
    client.disconnect();
    espClient.flush();

  }
}


void loop() {
  Serial.print("External Temperature: ");
  Serial.print(emc2101.getExternalTemperature());
  Serial.println(" degrees C");

  Serial.print("Internal Temperature: ");
  Serial.print(emc2101.getInternalTemperature());
  Serial.println(" degrees C");

  Serial.print("Duty Cycle: ");
  Serial.print(emc2101.getDutyCycle());
  Serial.print("% / Fan RPM: ");
  Serial.print(emc2101.getFanRPM());
  Serial.println(" RPM");
  Serial.println("");

  delay(5000);
}
