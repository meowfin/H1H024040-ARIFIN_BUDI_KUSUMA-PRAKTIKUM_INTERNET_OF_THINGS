#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;
const char* topicPerintah = "ahlele/ahlelas";

const int ledPin = D4;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;

  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  Serial.print("Pesan diterima [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(pesan);

  JsonDocument doc;

  DeserializationError error = deserializeJson(doc, pesan);

  if (error) {
    Serial.print("Gagal parsing JSON: ");
    Serial.println(error.c_str());
    return;
  }

  const char* perintah = doc["perintah"];

  // BARIS TAMBAHAN: mengambil nilai intensitas dari JSON
  int intensitas = doc["intensitas"] | 255;

  if (String(perintah) == "ON") {
    // BARIS DIUBAH: LED menyala sesuai nilai intensitas
    analogWrite(ledPin, intensitas);

    Serial.print("Aktuator: ON | Intensitas: ");
    Serial.println(intensitas);

  } else if (String(perintah) == "OFF") {
    // BARIS DIUBAH: LED dimatikan menggunakan nilai PWM 0
    analogWrite(ledPin, 0);

    Serial.println("Aktuator: OFF");
  }
}

void hubungkanWiFi() {
  WiFi.begin(ssid, password);

  Serial.print("Menghubungkan ke WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT() {
  while (!client.connected()) {

    Serial.print("Menghubungkan ke broker MQTT...");

    String clientId =
      "ESP32Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {

      Serial.println("berhasil terhubung!");

      client.subscribe(topicPerintah);

      Serial.print("Subscribe ke topic: ");
      Serial.println(topicPerintah);

    } else {

      Serial.print("gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 2 detik");

      delay(2000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin, LOW);

  hubungkanWiFi();

  client.setServer(mqttServer, mqttPort);

  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    hubungkanMQTT();
  }

  client.loop();
}
