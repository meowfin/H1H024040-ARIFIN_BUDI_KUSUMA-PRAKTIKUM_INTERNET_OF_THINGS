#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>

const char* ssid = "NAMA_WIFI";
const char* password = "PASSWORD_WIFI";

const char* mqttServer = "broker.hivemq.com";
const int mqttPort = 1883;

const char* topicData = "ahlele/ahlelasData";
const char* topicPerintah = "ahlele/ahlelasPerintah";

// BARIS TAMBAHAN: topic khusus untuk buzzer
const char* topicBuzzer = "ahlele/ahlelasBuzzer";

#define DHTPIN 4
#define DHTTYPE DHT22

const int ledPin = D4;

// BARIS TAMBAHAN: pin buzzer
const int buzzerPin = D5;

DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000;

void callback(char* topic, byte* payload, unsigned int length) {

  String pesan;

  for (unsigned int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  JsonDocument doc;

  if (deserializeJson(doc, pesan)) {
    return;
  }

  const char* perintah = doc["perintah"];

  // BARIS TAMBAHAN: pengecekan topic untuk LED
  if (String(topic) == topicPerintah) {

    digitalWrite(
      ledPin,
      String(perintah) == "ON" ? HIGH : LOW
    );

    Serial.print("Perintah LED diterima -> ");
    Serial.println(perintah);
  }

  // BARIS TAMBAHAN: pengecekan topic untuk buzzer
  else if (String(topic) == topicBuzzer) {

    digitalWrite(
      buzzerPin,
      String(perintah) == "ON" ? HIGH : LOW
    );

    Serial.print("Perintah Buzzer diterima -> ");
    Serial.println(perintah);
  }
}

void hubungkanWiFi() {

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("WiFi berhasil terhubung!");
}

void hubungkanMQTT() {

  while (!client.connected()) {

    String clientId =
      "ESP32Client-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {

      // Subscribe topic LED
      client.subscribe(topicPerintah);

      // BARIS TAMBAHAN: subscribe topic buzzer
      client.subscribe(topicBuzzer);

      Serial.println(
        "Terhubung dan subscribe topic LED dan Buzzer"
      );

    } else {

      delay(2000);
    }
  }
}

void setup() {

  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);

  // BARIS TAMBAHAN: mengatur pin buzzer sebagai output
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(ledPin, LOW);

  // BARIS TAMBAHAN: memastikan buzzer mati ketika awal
  digitalWrite(buzzerPin, LOW);

  dht.begin();

  hubungkanWiFi();

  client.setServer(mqttServer, mqttPort);

  client.setCallback(callback);
}

void loop() {

  if (!client.connected()) {
    hubungkanMQTT();
  }

  client.loop();

  if (millis() - waktuTerakhirPublish > intervalPublish) {

    waktuTerakhirPublish = millis();

    float suhu = dht.readTemperature();

    if (!isnan(suhu)) {

      JsonDocument doc;

      doc["suhu"] = suhu;

      char buffer[128];

      serializeJson(doc, buffer);

      client.publish(topicData, buffer);

      Serial.print("Data terkirim: ");
      Serial.println(buffer);
    }
  }
}
