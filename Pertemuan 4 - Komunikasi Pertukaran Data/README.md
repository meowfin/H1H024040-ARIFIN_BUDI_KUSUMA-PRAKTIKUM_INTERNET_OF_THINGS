# Modul 4: Komunikasi Pertukaran Data
## Library dan Dependencies
Library yang diperlukan pada praktikum:

```cpp
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <DHT.h>
```

Fungsi masing-masing library:

| Library | Fungsi |
|---|---|
| `ESP8266WiFi.h` | Menghubungkan ESP8266 ke jaringan WiFi |
| `PubSubClient.h` | Menangani komunikasi MQTT |
| `ArduinoJson.h` | Membuat dan membaca data JSON |
| `DHT.h` | Membaca sensor DHT11 |

---
# Percobaan Praktikum
## Percobaan 4A
```cpp
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
```
## Alur Percobaan 4A:

```text
ESP8266
   │
   ├── Terhubung WiFi
   │
   ├── Terhubung MQTT Broker
   │
   ├── Subscribe topicPerintah
   │
   ▼
Menunggu pesan
   │
   ▼
Pesan MQTT diterima
   │
   ▼
callback()
   │
   ▼
Payload → String
   │
   ▼
Deserialisasi JSON
   │
   ├── Gagal → tampilkan error → selesai
   │
   ▼
Ambil "perintah"
   │
   ├── ON → LED ON
   │
   └── OFF → LED OFF
```
---

## Percobaan 4B:
```cpp
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
```
## Alur Percobaan 4B:
```text
             ESP8266
                │
        ┌───────┴───────┐
        │               │
      DHT22            MQTT
        │               │
   Baca suhu       client.loop()
        │               │
        ▼               ▼
    JSON suhu       Pesan masuk
        │               │
        ▼               ▼
     PUBLISH         CALLBACK
        │               │
        ▼               ▼
   topicData       JSON perintah
                        │
                        ▼
                   ON / OFF
                        │
                        ▼
                       LED
```

---

# Pertanyaan Praktikum
## Modifikasi Percobaan 4A:
```cpp
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
```

---

## Penjelasan Baris yang Ditambahkan atau Diubah

### A. Membaca nilai intensitas

```cpp
int intensitas = doc["intensitas"] | 255;
```

Baris ini merupakan **baris tambahan**.

Fungsinya adalah mengambil nilai `intensitas` dari JSON yang diterima.

Contohnya jika MQTT menerima:

```json
{"perintah":"ON","intensitas":128}
```

maka:

```text
intensitas = 128
```

Angka `255` setelah tanda `|` digunakan sebagai nilai default. Artinya, jika JSON tidak memiliki parameter `intensitas`, sistem menggunakan nilai `255`.

Dengan demikian, JSON:

```json
{"perintah":"ON"}
```

tetap dapat digunakan dan LED akan menyala dengan intensitas maksimum.

---

### B. Mengubah kontrol LED

Kode sebelumnya:

```cpp
digitalWrite(ledPin, HIGH);
```

diubah menjadi:

```cpp
analogWrite(ledPin, intensitas);
```

`analogWrite()` digunakan untuk memberikan nilai PWM pada LED.

Rentang nilai yang digunakan:

| Nilai Intensitas | Kondisi LED |
|---:|---|
| 0 | Mati |
| 50 | Sangat redup |
| 128 | Sedang |
| 200 | Cukup terang |
| 255 | Maksimum |

Jadi, semakin besar nilai `intensitas`, semakin besar pula tingkat kecerahan LED.

---

### C. Mematikan LED

Pada kondisi `OFF`, digunakan:

```cpp
analogWrite(ledPin, 0);
```

Nilai `0` menghasilkan duty cycle PWM sebesar 0 sehingga LED dimatikan.

---

### D. Menampilkan intensitas pada Serial Monitor

```cpp
Serial.print("Aktuator: ON | Intensitas: ");
Serial.println(intensitas);
```

Kedua baris ini ditambahkan untuk menampilkan nilai intensitas yang sedang digunakan.

Contoh output:

```text
Aktuator: ON | Intensitas: 128
```

Hal ini memudahkan proses pengujian dan dokumentasi hasil praktikum.

---
## Contoh Pengujian MQTT

### LED dengan intensitas maksimum

Topic:

```text
ahlele/ahlelas
```

Payload:

```json
{"perintah":"ON","intensitas":255}
```

Hasil:

```text
LED menyala dengan intensitas maksimum.
```

### LED dengan intensitas sedang

```json
{"perintah":"ON","intensitas":128}
```

Hasil:

```text
LED menyala dengan intensitas sedang.
```

### LED dengan intensitas rendah

```json
{"perintah":"ON","intensitas":50}
```

Hasil:

```text
LED menyala dengan intensitas rendah.
```

### Mematikan LED

```json
{"perintah":"OFF"}
```

Hasil:

```text
LED mati.
```

---

## Modifikasi Percobaan 4B:
```cpp
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
```

---

## Penjelasan Baris yang Ditambahkan
### A. Menambahkan Topic Buzzer

```cpp
const char* topicBuzzer = "ahlele/ahlelasBuzzer";
```

Baris ini digunakan untuk membuat topic MQTT khusus yang digunakan untuk mengirim perintah kepada buzzer.

Dengan demikian, sistem mempunyai dua topic actuator:

```text
ahlele/ahlelasPerintah
```

untuk LED dan:

```text
ahlele/ahlelasBuzzer
```

untuk buzzer.

Pemisahan topic membuat perangkat dapat membedakan perintah untuk masing-masing actuator.

---

### B. Menentukan Pin Buzzer

```cpp
const int buzzerPin = D5;
```

Baris ini menentukan bahwa buzzer dihubungkan ke pin `D5` pada ESP8266.

Dengan adanya variabel ini, pin buzzer dapat digunakan di berbagai bagian program tanpa harus menuliskan `D5` secara berulang.

---

### C. Membedakan Topic LED dan Buzzer

Bagian berikut ditambahkan ke dalam fungsi `callback()`:

```cpp
if (String(topic) == topicPerintah) {
```

Kode tersebut memeriksa apakah pesan MQTT yang diterima berasal dari topic LED.

Jika benar, perintah akan diberikan kepada LED.

Kemudian ditambahkan:

```cpp
else if (String(topic) == topicBuzzer) {
```

Bagian tersebut digunakan untuk memeriksa apakah pesan berasal dari topic buzzer.

Jika benar, perintah akan diberikan kepada buzzer.

Dengan mekanisme tersebut, satu fungsi `callback()` dapat menangani beberapa topic sekaligus.

---

### D. Kontrol LED Berdasarkan Topic

```cpp
digitalWrite(
  ledPin,
  String(perintah) == "ON" ? HIGH : LOW
);
```

Kode tersebut tetap digunakan untuk mengontrol LED.

Jika nilai:

```json
{"perintah":"ON"}
```

maka LED diberi:

```cpp
HIGH
```

Jika nilai:

```json
{"perintah":"OFF"}
```

maka LED diberi:

```cpp
LOW
```

---

### E. Kontrol Buzzer

Kode berikut merupakan tambahan:

```cpp
digitalWrite(
  buzzerPin,
  String(perintah) == "ON" ? HIGH : LOW
);
```

Cara kerjanya sama seperti LED.

Jika:

```json
{"perintah":"ON"}
```

maka buzzer diberi `HIGH`.

Jika:

```json
{"perintah":"OFF"}
```

maka buzzer diberi `LOW`.

Modifikasi ini mengasumsikan buzzer yang digunakan merupakan **active buzzer** yang dapat dikontrol dengan kondisi HIGH/LOW.

---

### F. Subscribe Topic Buzzer

Pada fungsi `hubungkanMQTT()` ditambahkan:

```cpp
client.subscribe(topicBuzzer);
```

Baris tersebut membuat ESP8266 berlangganan atau **subscribe** ke topic buzzer.

Setelah berhasil subscribe, perangkat dapat menerima pesan yang dikirim ke:

```text
ahlele/ahlelasBuzzer
```

---

### G. Mengatur Pin Buzzer sebagai Output

Pada `setup()` ditambahkan:

```cpp
pinMode(buzzerPin, OUTPUT);
```

Fungsinya adalah mengatur pin `D5` sebagai output sehingga ESP8266 dapat memberikan sinyal kontrol kepada buzzer.

---

### H. Memastikan Buzzer Mati Saat Awal

```cpp
digitalWrite(buzzerPin, LOW);
```

Baris ini digunakan untuk memastikan buzzer berada dalam kondisi mati ketika ESP8266 pertama kali dijalankan.

---

## Contoh Pengujian Modifikasi 4B
### Pengujian LED
Gunakan topic:

```text
ahlele/ahlelasPerintah
```

Kirim:

```json
{"perintah":"ON"}
```

Hasil:

```text
LED menyala.
```

Kemudian kirim:

```json
{"perintah":"OFF"}
```

Hasil:

```text
LED mati.
```

---

### Pengujian Buzzer

Gunakan topic:

```text
ahlele/ahlelasBuzzer
```

Kirim:

```json
{"perintah":"ON"}
```

Hasil:

```text
Buzzer menyala.
```

Kemudian kirim:

```json
{"perintah":"OFF"}
```

Hasil:

```text
Buzzer mati.
```

---

## Dokumentasi
### Alat dan Bahan 
<img width="1280" height="960" alt="Alat dan Bahan Percobaan modul 4" src="https://github.com/user-attachments/assets/33a5f6f8-501a-44f0-94d7-4f219095bc60" />

### Percobaan 4A
<img width="1280" height="960" alt="percobaan 4a_IoT" src="https://github.com/user-attachments/assets/8c5e7aa8-64df-41ac-b547-418381c487c5" />

### Percobaan 4B
<img width="1202" height="1280" alt="Percobaan 4B_IoT" src="https://github.com/user-attachments/assets/799ac543-a52a-46f0-8a34-a19288e463cc" />

<img width="1280" height="960" alt="Percobaan 4B memakai LED" src="https://github.com/user-attachments/assets/562bec73-67f3-4321-943b-45fdd8170951" />


