#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ESP32Servo.h>


// WiFi credentials
const char* ssid = "หิวยัง";
const char* password = "11111111";

// MQTT credentials
const char* mqtt_server = "8df4daee0ae8481a8b0539623beb8aa0.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "hivemq.webclient.1747362479928";
const char* mqtt_password = "axy1&DLl409AT.fS>?Kj";

// MQTT topics
const char* topic_servo1 = "Slider1";
const char* topic_servo2 = "Slider2";
const char* topic_led = "LED";
const char* topic_buzzer = "BUZZER";

// GPIO Pins
const int servo1Pin = 27;  // MG995
const int servo2Pin = 24;  // MG90
const int ledPin = 2;
const int buzzerPin = 26;

Servo servo1;
Servo servo2;

WiFiClientSecure secureClient;
PubSubClient client(secureClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  if (String(topic) == topic_servo1) {
    int angle = msg.toInt();
    servo1.write(angle);
  } else if (String(topic) == topic_servo2) {
    int angle = msg.toInt();
    servo2.write(angle);
  } else if (String(topic) == topic_led) {
    if (msg == "ON") digitalWrite(ledPin, HIGH);
    else if (msg == "OFF") digitalWrite(ledPin, LOW);
  } else if (String(topic) == topic_buzzer) {
    if (msg == "ON") digitalWrite(buzzerPin, HIGH);
    else if (msg == "OFF") digitalWrite(buzzerPin, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      client.subscribe(topic_servo1);
      client.subscribe(topic_servo2);
      client.subscribe(topic_led);
      client.subscribe(topic_buzzer);
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // GPIO setup
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);

  // WiFi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // TLS config
  secureClient.setInsecure(); // Do not verify server cert

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
