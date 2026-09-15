# Modul 2 - Konfigurasi Jaringan

## Tujuan Praktikum

Praktikum ini bertujuan untuk memahami konfigurasi jaringan WiFi pada ESP8266, khususnya mode **Station (STA)**, **Access Point (AP)**, dan **AP + STA**. Praktikum juga bertujuan untuk menghubungkan ESP8266 ke jaringan WiFi, membuat jaringan Access Point, serta membaca informasi jaringan seperti IP Address, MAC Address, RSSI, dan jumlah perangkat yang terhubung.

---

## Library

Library yang digunakan dalam praktikum adalah:

```cpp
#include <ESP8266WiFi.h>
```

Library `ESP8266WiFi.h` digunakan untuk mengatur fungsi komunikasi WiFi pada ESP8266, termasuk mode Station, Access Point, dan AP + STA.

---

# Percobaan 2A – WiFi Station Mode

## 1. Deskripsi Percobaan

Pada percobaan ini, ESP8266 dikonfigurasi sebagai **Station (STA)**. ESP8266 berperan sebagai client yang terhubung ke jaringan WiFi yang telah tersedia.

Program juga menggunakan LED sebagai indikator status koneksi. Selain itu, ditambahkan fitur **auto reconnect** dan pembacaan RSSI secara berkala untuk memantau kondisi koneksi WiFi.

---

## 2. Kode Final

Kode berikut merupakan kode final yang digunakan setelah dilakukan penyesuaian selama praktikum.

```cpp
#include <ESP8266WiFi.h>                    // Library WiFi untuk ESP8266

const char* ssid     = "personalX";         // Nama jaringan WiFi
const char* password = "177013003";         // Password jaringan WiFi
const int ledPin = 2;                       // Pin LED indikator

void setup() {
  Serial.begin(115200);                     // Memulai komunikasi Serial
  pinMode(ledPin, OUTPUT);                   // Mengatur pin LED sebagai output
  digitalWrite(ledPin, LOW);                 // Mematikan LED saat awal

  WiFi.mode(WIFI_STA);                       // Mengatur ESP8266 sebagai Station
  WiFi.begin(ssid, password);                // Memulai koneksi ke WiFi

  WiFi.setAutoReconnect(true);               // Mengaktifkan reconnect otomatis
  Serial.print("Menghubungkan ke WiFi");

  while (WiFi.status() != WL_CONNECTED) {    // Menunggu sampai WiFi terhubung
    delay(500);                              // Jeda 500 ms
    Serial.print(".");                       // Menampilkan indikator koneksi
  }

  Serial.println();
  Serial.println("WiFi berhasil terhubung!");
  Serial.print("IP Address  : ");
  Serial.println(WiFi.localIP());            // Menampilkan IP Address
  Serial.print("MAC Address : ");
  Serial.println(WiFi.macAddress());         // Menampilkan MAC Address
  Serial.print("RSSI (dBm)  : ");
  Serial.println(WiFi.RSSI());               // Menampilkan kekuatan sinyal

  digitalWrite(ledPin, HIGH);                // Menyalakan LED saat terhubung
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {       // Mengecek status koneksi
    Serial.println("Status: Terhubung");
    digitalWrite(ledPin, HIGH);              // LED menyala jika terhubung
  } else {
    Serial.println("Status: Terputus");
    digitalWrite(ledPin, LOW);               // LED mati jika terputus
  }

  Serial.print("RSSI (dBm)  : ");
  Serial.println(WiFi.RSSI());               // Menampilkan RSSI terbaru
  delay(5000);                               // Update setiap 5 detik
}
```

## 3. Penjelasan Kode Final

### Library dan Konfigurasi

`ESP8266WiFi.h` digunakan untuk menyediakan fungsi WiFi pada ESP8266. Variabel `ssid` dan `password` digunakan untuk menyimpan informasi jaringan yang akan dihubungkan, sedangkan `ledPin` menentukan pin LED sebagai indikator koneksi.

### Konfigurasi Station

```cpp
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
```

`WIFI_STA` mengatur ESP8266 sebagai Station atau client, sedangkan `WiFi.begin()` digunakan untuk memulai koneksi ke jaringan WiFi.

### Auto Reconnect

```cpp
WiFi.setAutoReconnect(true);
```

Digunakan untuk mengaktifkan fitur koneksi ulang secara otomatis ketika koneksi WiFi terputus.

### Menunggu Koneksi

```cpp
while (WiFi.status() != WL_CONNECTED)
```

Perulangan digunakan agar program terus menunggu sampai ESP8266 berhasil terhubung ke jaringan WiFi.

### Informasi Jaringan

Program menampilkan:

- `WiFi.localIP()` → IP Address ESP8266.
- `WiFi.macAddress()` → MAC Address ESP8266.
- `WiFi.RSSI()` → kekuatan sinyal WiFi dalam dBm.

### Percabangan Status WiFi

```cpp
if (WiFi.status() == WL_CONNECTED)
```

Jika ESP8266 masih terhubung, LED dinyalakan dan status **Terhubung** ditampilkan. Jika tidak terhubung, LED dimatikan dan status **Terputus** ditampilkan.

---

## 4. Modifikasi Program

Modifikasi dilakukan untuk meningkatkan fungsi program dari kode dasar praktikum. Penyesuaian yang dilakukan adalah:

1. Menambahkan **LED sebagai indikator status koneksi**.
2. Menambahkan `WiFi.setAutoReconnect(true)` untuk mendukung koneksi ulang otomatis.
3. Menambahkan pengecekan status koneksi pada fungsi `loop()`.
4. Menampilkan nilai RSSI secara berkala.
5. Mengatur LED menyala ketika WiFi terhubung dan mati ketika WiFi terputus.

Dengan modifikasi tersebut, program tidak hanya melakukan koneksi awal, tetapi juga dapat memberikan informasi kondisi koneksi secara berkala.

---

# Percobaan 2B – Access Point + Station

## 1. Deskripsi Percobaan

Pada percobaan ini, ESP8266 dikonfigurasi menggunakan mode **AP + STA**. ESP8266 dapat terhubung ke jaringan WiFi utama sebagai Station sekaligus membuat jaringan WiFi sendiri sebagai Access Point.

Program menampilkan IP Address Station, SSID Access Point, IP Address Access Point, status koneksi Station, serta jumlah perangkat yang terhubung ke Access Point.

---

## 2. Kode Final

Kode berikut merupakan kode final yang digunakan setelah dilakukan modifikasi pada program percobaan.

```cpp
#include <ESP8266WiFi.h>                         // Library WiFi untuk ESP8266

const char* sta_ssid = "NamaWiFiRumah";          // SSID WiFi utama
const char* sta_password = "PasswordWiFi";       // Password WiFi utama
const char* ap_ssid = "ESP8266_AccessPoint";     // SSID Access Point
const char* ap_password = "12345678";            // Password Access Point

void setup() {
  Serial.begin(115200);                          // Memulai komunikasi Serial

  WiFi.mode(WIFI_AP_STA);                        // Mengaktifkan mode AP + STA

  WiFi.begin(sta_ssid, sta_password);             // Menghubungkan ke WiFi utama
  Serial.print("Menghubungkan ke WiFi");

  while (WiFi.status() != WL_CONNECTED) {        // Menunggu koneksi berhasil
    delay(500);                                  // Jeda 500 ms
    Serial.print(".");                           // Indikator proses koneksi
  }

  Serial.println();
  Serial.println("WiFi rumah berhasil terhubung!");
  Serial.print("IP STA : ");
  Serial.println(WiFi.localIP());                // Menampilkan IP Station

  WiFi.softAP(ap_ssid, ap_password);             // Membuat Access Point
  Serial.println("Access Point aktif!");

  Serial.print("SSID AP : ");
  Serial.println(ap_ssid);                       // Menampilkan SSID AP

  Serial.print("IP AP : ");
  Serial.println(WiFi.softAPIP());               // Menampilkan IP Access Point
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {            // Mengecek status Station
    Serial.println("Status STA: Terhubung");
    Serial.print("IP STA : ");
    Serial.println(WiFi.localIP());              // Menampilkan IP Station
  } else {
    Serial.println("Status STA: Terputus");      // Menampilkan status terputus
  }

  int jumlahClient = WiFi.softAPgetStationNum(); // Menghitung client AP
  Serial.print("Jumlah perangkat AP: ");
  Serial.println(jumlahClient);                  // Menampilkan jumlah client

  delay(5000);                                   // Update setiap 5 detik
}
```

> **Catatan:** Ganti `NamaWiFiRumah` dan `PasswordWiFi` dengan konfigurasi jaringan yang digunakan saat praktikum. Hindari memasukkan password WiFi pribadi ke repository publik.

---

## 3. Penjelasan Kode Final

### Konfigurasi AP + STA

```cpp
WiFi.mode(WIFI_AP_STA);
```

Perintah tersebut mengatur ESP8266 agar dapat bekerja sebagai **Station sekaligus Access Point**.

### Koneksi ke WiFi Utama

```cpp
WiFi.begin(sta_ssid, sta_password);
```

Digunakan untuk menghubungkan ESP8266 ke jaringan WiFi utama sebagai Station.

### Membuat Access Point

```cpp
WiFi.softAP(ap_ssid, ap_password);
```

Digunakan untuk membuat jaringan WiFi sendiri pada ESP8266 dengan SSID dan password yang telah ditentukan.

### IP Address

```cpp
WiFi.localIP();
WiFi.softAPIP();
```

`WiFi.localIP()` digunakan untuk memperoleh IP ESP8266 pada jaringan utama sebagai Station, sedangkan `WiFi.softAPIP()` digunakan untuk memperoleh IP Access Point.

### Mengecek Jumlah Client

```cpp
WiFi.softAPgetStationNum();
```

Digunakan untuk mengetahui jumlah perangkat yang sedang terhubung ke Access Point ESP8266.

### Percabangan Status

```cpp
if (WiFi.status() == WL_CONNECTED)
```

Digunakan untuk mengecek apakah koneksi ESP8266 sebagai Station masih aktif. Jika terhubung, IP STA ditampilkan. Jika tidak, program menampilkan status terputus.

---

## 4. Modifikasi Program

Modifikasi utama pada percobaan 2B adalah mengubah konfigurasi dari mode Access Point menjadi **AP + STA**, sehingga ESP8266 dapat menjalankan dua fungsi secara bersamaan.

Perubahan utama dilakukan dengan:

```cpp
WiFi.mode(WIFI_AP_STA);
```

Kemudian ditambahkan koneksi ke jaringan utama menggunakan:

```cpp
WiFi.begin(sta_ssid, sta_password);
```

dan pembuatan Access Point menggunakan:

```cpp
WiFi.softAP(ap_ssid, ap_password);
```

Program juga dimodifikasi untuk menampilkan **IP STA** dan **IP AP**, serta memantau jumlah perangkat yang terhubung menggunakan `WiFi.softAPgetStationNum()`.

---

# Perbandingan Mode WiFi

| Aspek | Station (STA) | Access Point (AP) | AP + STA |
|---|---|---|---|
| Peran ESP8266 | Client | Penyedia jaringan | Client + penyedia jaringan |
| Terhubung ke WiFi utama | Ya | Tidak | Ya |
| Membuat jaringan sendiri | Tidak | Ya | Ya |
| Fungsi | Terhubung ke jaringan | Menyediakan jaringan | Terhubung dan menyediakan jaringan |
| Mode program | `WIFI_STA` | `WIFI_AP` | `WIFI_AP_STA` |

---

# Hasil Pengamatan

## Percobaan 2A – Station

ESP8266 berhasil dikonfigurasi sebagai Station dan terhubung ke jaringan WiFi. Serial Monitor menampilkan IP Address, MAC Address, RSSI, serta status koneksi. LED juga digunakan sebagai indikator koneksi.

**Hasil pengamatan:**

- SSID: `personalX`
- IP Address: `.........................`
- MAC Address: `.........................`
- RSSI: `......................... dBm`
- Status: **Terhubung**

## Percobaan 2B – AP + STA

ESP8266 berhasil menjalankan mode AP + STA. ESP8266 dapat terhubung ke jaringan utama sebagai Station sekaligus membuat Access Point dengan SSID `ESP8266_AccessPoint`.

**Hasil pengamatan:**

- SSID STA: `NamaWiFiRumah`
- IP STA: `.........................`
- SSID AP: `ESP8266_AccessPoint`
- IP AP: `.........................`
- Jumlah perangkat AP: `.........................`
- Status STA: **Terhubung**

---

# Dokumentasi

## Alat dan Bahan
<img width="1280" height="960" alt="Alat dan bahan percobaan modul 2" src="https://github.com/user-attachments/assets/dcf52fec-aace-4a8b-b7c3-d9a009494715" />

## Percobaan 2A
<img width="1280" height="960" alt="Percobaan 2A_IoT (2)" src="https://github.com/user-attachments/assets/f0efdc4c-037b-4fd7-8408-3071d970b952" />

## Percobaan 2B
<img width="1280" height="960" alt="Percobaan 2B_IoT" src="https://github.com/user-attachments/assets/e13b5f3f-365f-4fa9-94fe-a1f8b7058587" />
