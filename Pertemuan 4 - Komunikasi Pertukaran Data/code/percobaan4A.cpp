#include <ESP8266WiFi.h>              // Library untuk menghubungkan ESP8266 ke jaringan WiFi
#include <PubSubClient.h>             // Library untuk komunikasi menggunakan protokol MQTT
#include <ArduinoJson.h>              // Library untuk membaca dan mengolah data JSON

const char* ssid = "personalX";       // Nama jaringan WiFi (SSID)
const char* password = "177013003";   // Password jaringan WiFi
const char* mqttServer = "broker.hivemq.com"; // Alamat broker MQTT yang digunakan
const int mqttPort = 1883;            // Port MQTT standar tanpa enkripsi
const char* topicPerintah = "ahlele/ahlelas"; // Topic MQTT untuk menerima perintah
const int ledPin = D4;                // Pin D4 ESP8266 digunakan untuk LED

WiFiClient espClient;                 // Membuat objek koneksi WiFi untuk komunikasi jaringan
PubSubClient client(espClient);       // Membuat objek MQTT menggunakan koneksi WiFi


// Fungsi callback dipanggil otomatis setiap ada pesan baru masuk
void callback(char* topic, byte* payload, unsigned int length) { // Fungsi untuk menangani pesan MQTT yang masuk
  String pesan;                       // Variabel untuk menyimpan pesan yang diterima

  for (unsigned int i = 0; i < length; i++) { // Melakukan perulangan sebanyak jumlah karakter pesan
    pesan += (char)payload[i];        // Mengubah setiap byte menjadi karakter lalu memasukkannya ke pesan
  }

  Serial.print("Pesan diterima [");   // Menampilkan teks pada Serial Monitor
  Serial.print(topic);                // Menampilkan nama topic tempat pesan diterima
  Serial.print("]: ");                // Menampilkan pemisah
  Serial.println(pesan);              // Menampilkan isi pesan yang diterima

  // Deserialisasi data JSON yang diterima
  JsonDocument doc;                   // Membuat dokumen JSON untuk menyimpan hasil parsing
  DeserializationError error = deserializeJson(doc, pesan); // Mengubah string JSON menjadi data yang dapat dibaca program

  if (error) {                        // Mengecek apakah terjadi kesalahan saat parsing JSON
    Serial.print("Gagal parsing JSON: "); // Menampilkan pesan kesalahan
    Serial.println(error.c_str());    // Menampilkan jenis kesalahan parsing
    return;                           // Menghentikan fungsi callback jika JSON tidak valid
  }

  const char* perintah = doc["perintah"]; // Mengambil nilai dari key "perintah" pada JSON

  if (String(perintah) == "ON") {     // Mengecek apakah nilai perintah adalah "ON"
    digitalWrite(ledPin, HIGH);       // Memberikan logika HIGH sehingga LED menyala
    Serial.println("Aktuator: ON");   // Menampilkan status aktuator ON
  } else if (String(perintah) == "OFF") { // Mengecek apakah nilai perintah adalah "OFF"
    digitalWrite(ledPin, LOW);        // Memberikan logika LOW sehingga LED mati
    Serial.println("Aktuator: OFF");  // Menampilkan status aktuator OFF
  }
}


// Fungsi untuk menghubungkan ESP8266 ke WiFi
void hubungkanWiFi() {                 // Mendefinisikan fungsi koneksi WiFi
  WiFi.begin(ssid, password);          // Memulai koneksi menggunakan SSID dan password
  Serial.print("Menghubungkan ke WiFi"); // Menampilkan proses koneksi pada Serial Monitor

  while (WiFi.status() != WL_CONNECTED) { // Mengulang selama ESP8266 belum terhubung ke WiFi
    delay(500);                       // Menunggu selama 500 milidetik
    Serial.print(".");                // Menampilkan titik sebagai indikator proses koneksi
  }

  Serial.println("\nWiFi berhasil terhubung!"); // Menampilkan pesan bahwa WiFi berhasil terhubung
}


// Fungsi untuk menghubungkan ESP8266 ke broker MQTT
void hubungkanMQTT() {                 // Mendefinisikan fungsi koneksi MQTT
  while (!client.connected()) {        // Mengulang selama ESP8266 belum terhubung ke broker MQTT
    Serial.print("Menghubungkan ke broker MQTT..."); // Menampilkan proses koneksi MQTT

    String clientId = "ESP32Client-" + String(random(0xffff), HEX); // Membuat ID client MQTT secara acak

    if (client.connect(clientId.c_str())) { // Mencoba menghubungkan client ke broker MQTT
      Serial.println("berhasil terhubung!"); // Menampilkan jika koneksi berhasil

      client.subscribe(topicPerintah); // Subscribe ke topic untuk menerima perintah MQTT
      Serial.print("Subscribe ke topic: "); // Menampilkan informasi topic yang digunakan
      Serial.println(topicPerintah);    // Menampilkan nama topic MQTT
    } else {                            // Jika koneksi MQTT gagal
      Serial.print("gagal, rc=");       // Menampilkan informasi kegagalan
      Serial.print(client.state());     // Menampilkan kode status koneksi MQTT
      Serial.println(" coba lagi dalam 2 detik"); // Memberikan informasi bahwa koneksi akan dicoba lagi
      delay(2000);                      // Menunggu 2 detik sebelum mencoba kembali
    }
  }
}


// Fungsi setup dijalankan satu kali saat ESP8266 mulai
void setup() {
  Serial.begin(115200);                // Memulai komunikasi Serial dengan baud rate 115200
  pinMode(ledPin, OUTPUT);              // Mengatur pin LED sebagai output
  digitalWrite(ledPin, LOW);            // Memastikan LED dalam kondisi mati saat awal
  hubungkanWiFi();                      // Memanggil fungsi untuk menghubungkan ESP8266 ke WiFi
  client.setServer(mqttServer, mqttPort); // Menentukan alamat dan port broker MQTT
  client.setCallback(callback);         // Mendaftarkan fungsi callback untuk menangani pesan masuk
}


// Fungsi loop dijalankan terus-menerus
void loop() {
  if (!client.connected()) {             // Mengecek apakah ESP8266 masih terhubung ke MQTT
    hubungkanMQTT();                     // Jika tidak terhubung, mencoba menghubungkan kembali
  }

  client.loop();                         // Memproses komunikasi MQTT dan menerima pesan masuk
}
