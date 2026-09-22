# Modul 3 - Protokol Komunikasi
## Library dan Dependencies
Library yang diperlukan pada praktikum:

### Percobaan 3A

```cpp
#include <ESP8266WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
```

Fungsi masing-masing library:

| Library              | Fungsi                                            |
| -------------------- | ------------------------------------------------- |
| `ESP8266WiFi.h`      | Digunakan untuk berinteraksi dengan WiFi          |
| `HTTPClient.h`       | Membuat komunikasi HTTP dari ESP32                |
| `WiFiClientSecure.h` | Menyediakan koneksi client untuk komunikasi HTTPS |
| `ArduinoJson.h`      | Membuat dan mengolah data dalam format JSON       |

### Percobaan 3B

```cpp
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
```

| Library          | Fungsi                                    |
| ---------------- | ----------------------------------------- |
| `ESP8266WiFi.h`  | Digunakan untuk berinteraksi dengan WiFi  |
| `PubSubClient.h` | Menangani komunikasi MQTT                 |
| `ArduinoJson.h`  | Membuat data dalam format JSON            |

---
# Percobaan Praktikum
## Percobaan 3A
```cpp
#include <ESP8266WiFi.h> // Library koneksi WiFi ESP8266
#include <ESP8266HTTPClient.h> // Library fungsi HTTP (POST/GET)
#include <WiFiClientSecure.h> // Library koneksi aman HTTPS
#include <ArduinoJson.h> // Library untuk format JSON


const char* ssid = "personalX"; // Nama WiFi
const char* password = "177013003"; // Password WiFi
const char* serverName = "https://httpbin.org/post"; // URL server tujuan


void setup() {
  Serial.begin(115200); // Memulai komunikasi serial dengan kecepatan 115200
  WiFi.begin(ssid, password); // Memulai proses sambung WiFi
 
  Serial.print("Menghubungkan ke WiFi"); // Cetak teks loading
  while (WiFi.status() != WL_CONNECTED) { // Ulangi terus selama WiFi belum nyambung
    delay(500); // Jeda 0,5 detik
    Serial.print("."); // Cetak titik (animasi loading)
  }
  serial.println();
  Serial.println("\nWiFi berhasil terhubung!"); // Cetak teks sukses
}


void loop() {
  if (WiFi.status() == WL_CONNECTED) { // Pastikan WiFi masih terhubung
    WiFiClientSecure client; // Buat klien untuk jalur HTTPS
    client.setInsecure(); // Matikan verifikasi sertifikat SSL (biar RAM nggak penuh)
   
    HTTPClient http; // Buat eksekutor HTTP
    http.begin(client, serverName); // Siapkan pengiriman ke URL tujuan
    http.addHeader("Content-Type", "application/json"); // Beri label bahwa paketnya berupa JSON


    JsonDocument doc; // Buat wadah JSON
    doc["suhu"] = 28.5; // Isi variabel suhu
    doc["kelembaban"] = 65.0; // Isi variabel kelembaban
   
    String requestBody; // Siapkan variabel teks kosong
    serializeJson(doc, requestBody); // Ubah JSON jadi format teks (String)


    Serial.print("\nMengirim data: ");
    Serial.println(requestBody);       //nyetak data JSON-nya
   
    int httpResponseCode = http.POST(requestBody); // Kirim data (POST) & catat kode balasannya
   
    Serial.print("Kode Response HTTP: "); // Cetak label balasan
    Serial.println(httpResponseCode); // Cetak angka balasan (contoh: 200 = Sukses)


    if (httpResponseCode > 0) { // Jika pengiriman jalan (tidak error/putus)
      String response = http.getString(); // Sedot teks balasan dari server
      Serial.println("Isi Response:"); // Cetak teks label
      Serial.println(response); // Tampilkan balasan dari server
    } else { // Jika gagal konek ke server
      Serial.print("Error saat mengirim POST: "); // Cetak label error
      Serial.println(http.errorToString(httpResponseCode).c_str()); // Cetak alasan error-nya
    }
   
    http.end(); // Putus koneksi HTTP agar memori alat tidak bocor/hang
  }
 
  delay(10000); // Jeda 10 detik sebelum mengulang ke atas
}
```
## Alur percobaan 3A:

```text
ESP32 mulai
     │
     ▼
Inisialisasi Serial
     │
     ▼
Hubungkan ke WiFi
     │
     ▼
WiFi berhasil?
   ┌─┴─┐
  Tidak Ya
   │    │
   │    ▼
   │  Buat JSON
   │    │
   │    ▼
   │  HTTP POST
   │    │
   │    ▼
   │  Terima Response
   │    │
   └────┤
        ▼
      Delay
        │
        └──────► Ulangi
```
---


## Percobaan 3B
```cpp
#include <ESP8266WiFi.h> // Library koneksi WiFi
#include <PubSubClient.h> // Library komunikasi MQTT
#include <ArduinoJson.h> // Library format JSON


const char* ssid = "personalX"; // Nama WiFi
const char* password = "177013003"; // Password WiFi


const char* mqttServer = "broker.hivemq.com"; // Alamat server broker MQTT
const int mqttPort = 1883; // Port standar MQTT
const char* mqttTopic = "ahlele/ahlelas"; // Jalur tujuan pengiriman data


WiFiClient espClient; // Buat kendaraan WiFi biasa
PubSubClient client(espClient); // Masukkan WiFi ke sistem MQTT


void hubungkanWiFi() {
  WiFi.begin(ssid, password); // Mulai sambung WiFi
  Serial.print("Menghubungkan ke WiFi"); // Cetak label
  while (WiFi.status() != WL_CONNECTED) { // Ulangi kalau belum nyambung
    delay(500); // Jeda 0,5 detik
    Serial.print("."); // Cetak titik loading
  }
  Serial.println("\nWiFi berhasil terhubung!"); // Cetak sukses
}


void hubungkanMQTT() {
  while (!client.connected()) { // Ulangi selama MQTT terputus
    Serial.print("Menghubungkan ke broker MQTT..."); // Cetak label
    String clientId = "ESP8266Client-" + String(random(0xffff), HEX); // Buat ID unik acak


    if (client.connect(clientId.c_str())) { // Coba login ke broker pakai ID tadi
      Serial.println("berhasil terhubung!"); // Jika sukses
    } else {
      Serial.print("gagal, rc="); // Jika gagal
      Serial.print(client.state()); // Cetak kode error dari MQTT
      Serial.println(" coba lagi dalam 2 detik"); // Cetak info jeda
      delay(2000); // Tunggu 2 detik sebelum coba lagi
    }
  }
}


void setup() {
  Serial.begin(115200); // Mulai komunikasi serial
  hubungkanWiFi(); // Panggil fungsi sambung WiFi
  client.setServer(mqttServer, mqttPort); // Kunci alamat dan port broker MQTT
}


void loop() {
  if (!client.connected()) { // Jika koneksi MQTT putus di tengah jalan
    hubungkanMQTT(); // Panggil fungsi sambung ulang
  }
  client.loop(); // Jaga detak jantung alat agar tetap "Online" di server


  // Membuat data sensor dalam format JSON
  JsonDocument doc; // Siapkan wadah JSON
  doc["suhu"] = 28.5; // Isi nilai suhu
  doc["kelembaban"] = 65.0; // Isi nilai kelembaban


  char buffer[128]; // Siapkan memori penyimpan teks
  serializeJson(doc, buffer); // Ubah bungkus JSON jadi teks dan simpan ke buffer


  // Mempublikasikan data ke topic MQTT
  client.publish(mqttTopic, buffer); // Kirim (publish) data dari buffer ke topik
  Serial.print("Data terkirim ke topic "); // Cetak label
  Serial.print(mqttTopic); // Cetak nama topik
  Serial.print(": "); // Cetak pemisah
  Serial.println(buffer); // Cetak isi data JSON-nya


  delay(10000); // Kirim data setiap 10 detik
}
```
## Alur Percobann 3B:
```text
ESP32
  │
  │ Connect WiFi
  ▼
WiFi
  │
  │ Connect MQTT
  ▼
MQTT Broker
  │
  │ Publish JSON
  ▼
Topic MQTT
  │
  ▼
MQTT Client
  │
  └── Subscribe Topic
```
---
 
# Pertanyaan Praktikum
## Modifikasi Percobaan 3A
```cpp
#include <WiFi.h>                 
#include <HTTPClient.h>           
#include <WiFiClientSecure.h>     
#include <ArduinoJson.h>          

const char* ssid = "namaWifi";                  
const char* password = "Password";              
const char* serverName = "https://httpbin.org/post"; 

void setup() {
  Serial.begin(115200);            
  WiFi.begin(ssid, password);      

  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("\nWiFi berhasil terhubung!");
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");

    JsonDocument doc;
    doc["suhu"] = 28.5;
    doc["kelembaban"] = 65.0;
    doc["iniWaktu"] = millis();

    String requestBody;
    serializeJson(doc, requestBody);

    Serial.print("\nMengirim data: ");
    Serial.println(requestBody);

    int httpResponseCode = http.POST(requestBody);

    Serial.print("Kode Response HTTP: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Isi Response:");
      Serial.println(response);
    } else {
      Serial.print("Error saat mengirim POST: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    http.end();
  }

  delay(10000);
}
```
## Penjelasan Modifikasi Program Percobaan 3A

## 1. Memanggil Library

```cpp
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
```

Library digunakan untuk menyediakan fungsi WiFi, HTTP, HTTPS, dan JSON.

---

## 2. Konfigurasi WiFi dan Server

```cpp
const char* ssid = "namaWifi";
const char* password = "Password";
const char* serverName = "https://httpbin.org/post";
```

Variabel tersebut menyimpan nama jaringan WiFi, password WiFi, dan alamat server tujuan.

---

## 3. Fungsi `setup()`

```cpp
void setup()
```

Fungsi `setup()` dijalankan **satu kali** ketika ESP32 mulai menyala atau melakukan reset.

Di dalamnya dilakukan:

* Memulai komunikasi Serial.
* Menghubungkan ESP32 ke WiFi.
* Menampilkan status koneksi pada Serial Monitor.

---

## 4. `Serial.begin()`

```cpp
Serial.begin(115200);
```

Digunakan untuk memulai komunikasi serial dengan baud rate:

```text
115200
```

Serial Monitor digunakan untuk melihat proses koneksi WiFi, data JSON, response HTTP, dan error.

---

## 5. Menghubungkan ESP32 ke WiFi

```cpp
WiFi.begin(ssid, password);
```

Perintah tersebut memulai proses koneksi ESP32 ke jaringan WiFi menggunakan SSID dan password yang telah ditentukan.

---

## 6. Percabangan `while`

```cpp
while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}
```

Conditional ini digunakan untuk memastikan ESP32 tetap menunggu sampai berhasil terhubung ke WiFi.

Jika:

```text
WiFi.status() != WL_CONNECTED
```

maka ESP32 masih belum terhubung dan program akan terus menunggu.

Ketika status berubah menjadi:

```text
WL_CONNECTED
```

maka perulangan berhenti dan program melanjutkan proses berikutnya.

---

## 7. `WiFiClientSecure`

```cpp
WiFiClientSecure client;
client.setInsecure();
```

`WiFiClientSecure` digunakan karena alamat server menggunakan:

```text
HTTPS
```

Sedangkan:

```cpp
client.setInsecure();
```

digunakan agar koneksi HTTPS tidak melakukan verifikasi sertifikat server.

Penggunaan ini sesuai untuk pengujian praktikum, tetapi untuk sistem nyata sebaiknya menggunakan verifikasi sertifikat yang sesuai.

---

## 8. Membuat HTTP Client

```cpp
HTTPClient http;
http.begin(client, serverName);
```

Objek `HTTPClient` digunakan untuk membuat komunikasi HTTP.

`http.begin()` menentukan client dan alamat server yang menjadi tujuan request.

---

## 9. Menentukan Content-Type

```cpp
http.addHeader("Content-Type", "application/json");
```

Baris tersebut memberitahu server bahwa isi data yang dikirimkan menggunakan format:

```text
application/json
```

Dengan demikian server dapat mengetahui bagaimana body request harus diproses.

---

## 10. Membuat Data JSON

```cpp
JsonDocument doc;

doc["suhu"] = 28.5;
doc["kelembaban"] = 65.0;
doc["iniWaktu"] = millis();
```

`JsonDocument` digunakan sebagai tempat penyimpanan struktur JSON.

Data yang dimasukkan terdiri dari:

| Key          |      Nilai | Keterangan                                      |
| ------------ | ---------: | ----------------------------------------------- |
| `suhu`       |     `28.5` | Contoh suhu dalam °C                            |
| `kelembaban` |     `65.0` | Contoh kelembaban dalam %                       |
| `iniWaktu`   | `millis()` | Waktu sejak ESP32 mulai menyala dalam milidetik |

---

## 11. Fungsi `millis()`

```cpp
doc["iniWaktu"] = millis();
```

`millis()` digunakan untuk memperoleh waktu dalam **milidetik sejak ESP32 mulai menjalankan program**.

Contoh data yang dihasilkan:

```json
{
  "suhu": 28.5,
  "kelembaban": 65.0,
  "iniWaktu": 10234
}
```

Nilai `iniWaktu` akan terus bertambah selama ESP32 berjalan.

Modifikasi ini dilakukan untuk menjawab pertanyaan praktikum mengenai penambahan waktu ke dalam data JSON.

---

## 12. Serialisasi JSON

```cpp
String requestBody;
serializeJson(doc, requestBody);
```

`serializeJson()` mengubah objek JSON yang terdapat pada `doc` menjadi string sehingga dapat dikirim sebagai HTTP request body.

---

## 13. Mengirim HTTP POST

```cpp
int httpResponseCode = http.POST(requestBody);
```

Baris tersebut mengirim data JSON menggunakan metode:

```text
HTTP POST
```

ke server yang telah ditentukan.

Nilai hasil proses tersebut disimpan dalam:

```cpp
httpResponseCode
```

---

## 14. Percabangan Response HTTP

```cpp
if (httpResponseCode > 0) {
```

Program memeriksa apakah proses request menghasilkan response.

Jika:

```text
httpResponseCode > 0
```

maka response dapat dibaca.

Program kemudian mengambil isi response:

```cpp
String response = http.getString();
```

Jika nilai tidak lebih dari 0, program menampilkan pesan error:

```cpp
Serial.print("Error saat mengirim POST: ");
```

Error kemudian diterjemahkan menggunakan:

```cpp
http.errorToString(httpResponseCode)
```

---

## 15. Mengakhiri HTTP Connection

```cpp
http.end();
```

Digunakan untuk mengakhiri komunikasi HTTP setelah proses request dan response selesai.

---

## 16. Delay

```cpp
delay(10000);
```

ESP32 menunggu selama:

```text
10.000 ms = 10 detik
```

sebelum melakukan pengiriman berikutnya.

---

# Dokumentasi

## Alat dan Bahan

<img width="1280" height="960" alt="Alat dan bahan percobaan modul 3" src="https://github.com/user-attachments/assets/b2f4423d-f423-4d57-9f94-a8e121c4ede0" />


## Percobaan 3A

<img width="899" height="1599" alt="percobaan 3A_IoT" src="https://github.com/user-attachments/assets/d681d061-b75f-441f-8c85-e437745f1fa7" />
<img width="478" height="850" alt="Video percobaan 3A" src="https://github.com/user-attachments/assets/6d882383-e5a9-4548-a01f-439b950fd28a" />

## Percobaan 3B

<img width="1280" height="960" alt="percobaan 3B_IoT" src="https://github.com/user-attachments/assets/4bb97333-c75b-4360-b1f1-67f230333fc9" />
<img width="800" height="450" alt="Video percobaan 3B" src="https://github.com/user-attachments/assets/801ba269-3bd1-4368-b299-d897fbc3cc57" />



