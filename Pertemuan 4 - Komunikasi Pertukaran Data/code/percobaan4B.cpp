#include <ESP8266WiFi.h>                  // Library untuk menghubungkan ESP8266 ke jaringan WiFi
#include <PubSubClient.h>                 // Library untuk komunikasi menggunakan protokol MQTT
#include <ArduinoJson.h>                  // Library untuk membaca dan membuat data JSON
#include <DHT.h>                          // Library untuk membaca sensor DHT

const char* ssid = "personalX";            // Nama jaringan WiFi (SSID)
const char* password = "177013003";        // Password jaringan WiFi

const char* mqttServer = "broker.hivemq.com"; // Alamat broker MQTT
const int mqttPort = 1883;                 // Port MQTT standar tanpa enkripsi
const char* topicData = "ahlele/ahlelasData"; // Topic untuk mengirim data suhu
const char* topicPerintah = "ahlele/ahlelasPerintah"; // Topic untuk menerima perintah

#define DHTPIN 4                           // Pin GPIO 4 digunakan untuk sensor DHT
#define DHTTYPE DHT22                      // Menentukan jenis sensor yang digunakan adalah DHT22

const int ledPin = D4;                     // Pin D4 digunakan untuk LED
DHT dht(DHTPIN, DHTTYPE);                  // Membuat objek sensor DHT dengan pin dan tipe yang sudah ditentukan

WiFiClient espClient;                      // Membuat objek koneksi WiFi
PubSubClient client(espClient);            // Membuat objek MQTT menggunakan koneksi WiFi


unsigned long waktuTerakhirPublish = 0;    // Menyimpan waktu terakhir data sensor dikirim
const long intervalPublish = 5000;         // Interval pengiriman data adalah 5000 ms atau 5 detik
                                            // Pengiriman dilakukan secara non-blocking menggunakan millis()


// Fungsi callback dijalankan otomatis ketika ada pesan MQTT yang masuk
void callback(char* topic, byte* payload, unsigned int length) { // Fungsi untuk menangani pesan MQTT masuk

  String pesan;                            // Variabel untuk menyimpan isi pesan MQTT

  for (unsigned int i = 0; i < length; i++) // Mengulang sebanyak jumlah karakter pesan
    pesan += (char)payload[i];             // Mengubah setiap byte menjadi karakter dan memasukkannya ke pesan

  JsonDocument doc;                        // Membuat dokumen untuk menampung data JSON

  if (deserializeJson(doc, pesan)) return; // Melakukan parsing JSON dan keluar jika parsing gagal

  const char* perintah = doc["perintah"];  // Mengambil nilai dari key "perintah" pada JSON

  digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
                                            // Jika perintah = ON, LED menyala
                                            // Jika perintah bukan ON, LED dimatikan
                                            // Operator ?: digunakan sebagai bentuk if-else singkat

  Serial.print("Perintah diterima -> Aktuator: "); // Menampilkan teks pada Serial Monitor
  Serial.println(perintah);                // Menampilkan nilai perintah yang diterima
}


// Fungsi untuk menghubungkan ESP8266 ke WiFi
void hubungkanWiFi() {                      // Mendefinisikan fungsi koneksi WiFi

  WiFi.begin(ssid, password);               // Memulai koneksi menggunakan SSID dan password

  while (WiFi.status() != WL_CONNECTED)    // Mengulang selama ESP8266 belum terhubung ke WiFi
    delay(500);                             // Menunggu 500 ms sebelum mengecek kembali

  Serial.println("WiFi berhasil terhubung!"); // Menampilkan pesan jika WiFi berhasil terhubung
}


// Fungsi untuk menghubungkan ESP8266 ke broker MQTT
void hubungkanMQTT() {                      // Mendefinisikan fungsi koneksi MQTT

  while (!client.connected()) {              // Mengulang selama ESP8266 belum terhubung ke broker

    String clientId = "ESP32Client-" + String(random(0xffff), HEX);
                                            // Membuat ID client MQTT secara acak

    if (client.connect(clientId.c_str())) { // Mencoba menghubungkan client ke broker MQTT

      client.subscribe(topicPerintah);      // Subscribe ke topic untuk menerima perintah

      Serial.println("Terhubung dan subscribe topic perintah");
                                            // Menampilkan pesan jika koneksi berhasil

    } else {                                // Jika koneksi MQTT gagal

      delay(2000);                          // Menunggu 2 detik sebelum mencoba kembali
    }
  }
}


// Fungsi setup dijalankan satu kali ketika ESP8266 mulai
void setup() {

  Serial.begin(115200);                     // Memulai komunikasi Serial dengan baud rate 115200

  pinMode(ledPin, OUTPUT);                  // Mengatur pin LED sebagai output

  dht.begin();                              // Menginisialisasi sensor DHT22

  hubungkanWiFi();                          // Menghubungkan ESP8266 ke jaringan WiFi

  client.setServer(mqttServer, mqttPort);   // Menentukan alamat dan port broker MQTT

  client.setCallback(callback);             // Menentukan fungsi callback untuk menangani pesan masuk
}


// Fungsi loop dijalankan berulang-ulang selama ESP8266 aktif
void loop() {

  if (!client.connected())                   // Mengecek apakah ESP8266 masih terhubung ke MQTT
    hubungkanMQTT();                         // Jika tidak terhubung, lakukan koneksi ulang

  client.loop();                             // Memproses komunikasi MQTT dan pesan yang masuk


  // Publish data sensor secara berkala tanpa memblokir proses subscribe
  if (millis() - waktuTerakhirPublish > intervalPublish) {
                                            // Mengecek apakah sudah lewat 5 detik sejak publish terakhir

    waktuTerakhirPublish = millis();         // Menyimpan waktu saat ini sebagai waktu publish terakhir

    float suhu = dht.readTemperature();      // Membaca suhu dari sensor DHT22 dalam derajat Celsius

    if (!isnan(suhu)) {                      // Mengecek apakah hasil pembacaan suhu valid

      JsonDocument doc;                     // Membuat dokumen JSON baru

      doc["suhu"] = suhu;                    // Memasukkan nilai suhu ke key "suhu"

      char buffer[128];                      // Membuat array karakter untuk menyimpan JSON

      serializeJson(doc, buffer);            // Mengubah data JSON menjadi format string

      client.publish(topicData, buffer);     // Mengirim data JSON ke topicData melalui MQTT

      Serial.print("Data terkirim: ");      // Menampilkan teks pada Serial Monitor

      Serial.println(buffer);                // Menampilkan data JSON yang berhasil dikirim
    }
  }
}
