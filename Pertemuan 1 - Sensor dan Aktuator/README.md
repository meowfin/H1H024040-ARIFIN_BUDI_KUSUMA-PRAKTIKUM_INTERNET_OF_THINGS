# Modul 1 - Sensor Dan Aktuator
## Library dan Dependencies
Library yang digunakan adalah DHT Sensor Library.

Library dimasukkan menggunakan:

```#include <DHT.h>```

Library ini digunakan untuk mempermudah proses komunikasi antara ESP32 dengan sensor DHT11, termasuk pembacaan suhu dan kelembapan.

---

# Percobaan Praktikum
## Percobaan 1A
```cpp
#include <DHT.h>
#define DHTPIN 2 // Pin GPIO untuk DHT
#define DHTTYPE DHT11 // Tipe dari DHT
DHT dht(DHTPIN, DHTTYPE); // Instane DHT

// Fungsi yang akan di jalankan saat pertama kali esp menyala
// Menginisiasi Serial dan DHT
void setup()
{
    Serial.begin(115200);
    dht.begin();
    Serial.println("Memulai akuisisi data sensor DHT11...");
}

// Fungsi yang akan di jalankan berulang
void loop()
{
    // Variabel penampung data dari DHT
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();

    // Melakukan pengecekan
    // Jika kelembaban dan suhu bernilai NaN, maka masuk block if
    // Jika false maka masuk block else
    if (isnan(kelembaban) || isnan(suhu))
    { // Menampilkan pesan error ke serial monitor
        Serial.println("Gagal membaca data dari sensor DHT11!");
    }
    else
    { // Menampilkan suhu dan kelembaban ke serial monitor
        Serial.print("Suhu: ");
        Serial.print(suhu);
        Serial.print(" °C, Kelembaban: ");
        Serial.print(kelembaban);
        Serial.println(" %");
    }
    delay(2000); // Jeda 2 detik
}
```
## Percobaan 2A
```cpp
#include <DHT.h>
#define DHTPIN 2 // Pin GPIO untuk DHT
#define DHTTYPE DHT11 // Tipe dari DHT
#define RELAYPIN 16 // Pin GPIO untuk Relay
DHT dht(DHTPIN, DHTTYPE); // Instance DHT
const float suhuThreshold = 30.0; // Batas ambang suhu

// Fungsi yang akan di jalankan saat pertama kali esp menyala
// Menginisiasi Serial, DHT, pin Relay dan nilai awal pin Relay
void setup()
{
    Serial.begin(115200);
    dht.begin();
    pinMode(RELAYPIN, OUTPUT);
    digitalWrite(RELAYPIN, LOW);
}

// Fungsi yang akan di jalankan berulang
void loop()
{
    float suhu = dht.readTemperature(); // Variabel suhu

    // Melakukan pengecekan
    // Jika suhu bernilai NaN, maka masuk block if
    // Jika false maka masuk block else
    if (isnan(suhu))
    { // Menampilkan pesan error ke serial monitor
        Serial.println("Gagal membaca data sensor!");
    }
    else
    { // Menampilkan suhu dan state dari Relay
        Serial.print("Suhu: ");
        Serial.print(suhu);
        Serial.print(" °C -> ");
        if (suhu > suhuThreshold)
        { // Relay menyala jika suhu lebih besar dari batas
            digitalWrite(RELAYPIN, HIGH);
            Serial.println("Aktuator: ON");
        }
        else
        { // Relay mati jika suhu kurang dari batas
            digitalWrite(RELAYPIN, LOW);
            Serial.println("Aktuator: OFF");
        }
    }
}
```
# Pertanyaan Praktikum
## Modifikasi Percobaan 1A
```cpp
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const int TOTAL_SAMPLE = 5;

void setup()
{
    Serial.begin(115200);
    dht.begin();
    Serial.println("Memulai akuisisi data sensor DHT11...");
}

void loop()
{
    float totalKelembaban = 0;
    float totalSuhu = 0;
    int valid = 0;

    for (int i = 0; i < TOTAL_SAMPLE; i++)
    {
        float suhu = dht.readTemperature();
        float kelembaban = dht.readHumidity();

        if (!isnan(suhu) && !isnan(kelembaban))
        {
            totalKelembaban += kelembaban;
            totalSuhu += suhu;
            valid++;
        }

        delay(100);
    }

    if (valid > 0)
    {
        Serial.printf(
            "Suhu: %.1f°C | Kelembaban: %.1f%%\n",
            totalSuhu / valid,
            totalKelembaban / valid
        );
    }
    else
    {
        Serial.println("Gagal membaca sensor DHT11");
    }

    delay(2000);
}
```
---

##  Penjelasan Program Percobaan 1A

### `#include <DHT.h>`

Memanggil library DHT agar ESP32 dapat berkomunikasi dengan sensor DHT11.

### `#define DHTPIN 2`

Menentukan GPIO 2 sebagai pin data sensor DHT11.

### `#define DHTTYPE DHT11`

Menentukan jenis sensor yang digunakan adalah DHT11.

### `DHT dht(DHTPIN, DHTTYPE);`

Membuat objek `dht` berdasarkan pin dan jenis sensor yang telah ditentukan.

### `const int TOTAL_SAMPLE = 5;`

Menentukan jumlah pembacaan sensor yang dilakukan, yaitu lima kali.

---

## 1. Fungsi `setup()`

```cpp
void setup()
```

Fungsi yang dijalankan satu kali ketika ESP32 mulai bekerja.

```cpp
Serial.begin(115200);
```

Memulai komunikasi Serial dengan baud rate 115200.

```cpp
dht.begin();
```

Menginisialisasi sensor DHT11.

```cpp
Serial.println("Memulai akuisisi data sensor DHT11...");
```

Menampilkan informasi awal pada Serial Monitor.

---

## 2. Fungsi `loop()`

```cpp
void loop()
```

Fungsi utama yang dijalankan berulang kali oleh ESP32.

```cpp
float totalKelembaban = 0;
float totalSuhu = 0;
int valid = 0;
```

Ketiga variabel digunakan untuk menyimpan jumlah suhu, jumlah kelembapan, dan jumlah data sensor yang berhasil dibaca.

---

## 3. Perulangan Pembacaan Sensor

```cpp
for (int i = 0; i < TOTAL_SAMPLE; i++)
```

Digunakan untuk melakukan pembacaan sensor sebanyak lima kali.

```cpp
float suhu = dht.readTemperature();
float kelembaban = dht.readHumidity();
```

Membaca nilai suhu dan kelembapan dari DHT11.

---

## 4. Pemeriksaan Data

```cpp
if (!isnan(suhu) && !isnan(kelembaban))
```

Digunakan untuk memastikan bahwa data suhu dan kelembapan yang diperoleh valid.

Jika data valid:

```cpp
totalKelembaban += kelembaban;
totalSuhu += suhu;
valid++;
```

Nilai sensor ditambahkan ke total dan jumlah data valid bertambah satu.

```cpp
delay(100);
```

Memberikan jeda 100 ms antar proses pembacaan dalam perulangan.

---

## 5. Menghitung Nilai Rata-rata

```cpp
if (valid > 0)
```

Memastikan terdapat setidaknya satu data sensor yang valid.

```cpp
totalSuhu / valid
```

Menghasilkan rata-rata suhu.

```cpp
totalKelembaban / valid
```

Menghasilkan rata-rata kelembapan.

Hasil kemudian ditampilkan menggunakan:

```cpp
Serial.printf(...)
```

---

## Alur Percobaan 1A

```text
Mulai
  ↓
Inisialisasi DHT11
  ↓
Baca suhu dan kelembapan
  ↓
Data valid?
 ├── Tidak → Lewati data
 └── Ya → Simpan data
  ↓
Sudah 5 kali pembacaan?
 ├── Tidak → Baca kembali
 └── Ya
  ↓
Hitung rata-rata
  ↓
Tampilkan suhu dan kelembapan
  ↓
Delay 2 detik
  ↓
Ulangi
```

---

## Modifikasi Percobaan 2A
```cpp
#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define RELAYPIN 16

DHT dht(DHTPIN, DHTTYPE);

const float suhuThresholdHigh = 30.0;
const float suhuThresholdLow = 28.0;

bool aktuator = false;

void setup()
{
    Serial.begin(115200);
    dht.begin();

    pinMode(RELAYPIN, OUTPUT);
    digitalWrite(RELAYPIN, LOW);
}

void loop()
{
    float suhu = dht.readTemperature();

    if (isnan(suhu))
    {
        Serial.println("Gagal membaca data sensor!");
    }
    else
    {
        Serial.print("Suhu: ");
        Serial.print(suhu);
        Serial.print(" °C -> ");

        if (!aktuator && suhu > suhuThresholdHigh)
        {
            aktuator = true;
            digitalWrite(RELAYPIN, HIGH);
            Serial.println("Aktuator: ON (melewati 30°C)");
        }
        else if (aktuator && suhu < suhuThresholdLow)
        {
            aktuator = false;
            digitalWrite(RELAYPIN, LOW);
            Serial.println("Aktuator: OFF (di bawah 28°C)");
        }
        else
        {
            if (aktuator)
            {
                Serial.println("Aktuator: ON (di zona histerisis)");
            }
            else
            {
                Serial.println("Aktuator: OFF (di zona histerisis)");
            }
        }
    }

    delay(2000);
}
```

---

## Penjelasan Program Percobaan 2A
## Konfigurasi

```cpp
#define DHTPIN 2
```

Menentukan GPIO 2 sebagai pin data DHT11.

```cpp
#define DHTTYPE DHT11
```

Menentukan jenis sensor yang digunakan.

```cpp
#define RELAYPIN 16
```

Menentukan GPIO 16 sebagai pin kendali relay.

---

## Nilai Threshold

```cpp
const float suhuThresholdHigh = 30.0;
const float suhuThresholdLow = 28.0;
```

Kedua variabel tersebut merupakan batas atas dan batas bawah pada sistem histerisis.

- `suhuThresholdHigh` = 30°C
- `suhuThresholdLow` = 28°C

---

## Status Aktuator

```cpp
bool aktuator = false;
```

Variabel ini menyimpan kondisi aktuator.

- `false` = OFF
- `true` = ON

Status ini diperlukan agar sistem dapat mempertahankan kondisi aktuator ketika suhu berada di antara 28°C dan 30°C.

---

## 1. Fungsi `setup()`

```cpp
Serial.begin(115200);
```

Memulai komunikasi Serial Monitor.

```cpp
dht.begin();
```

Menginisialisasi sensor DHT11.

```cpp
pinMode(RELAYPIN, OUTPUT);
```

Mengatur GPIO 16 sebagai output untuk mengendalikan relay.

```cpp
digitalWrite(RELAYPIN, LOW);
```

Memastikan relay berada dalam kondisi OFF ketika ESP32 mulai bekerja.

---

## 2. Fungsi `loop()`

```cpp
float suhu = dht.readTemperature();
```

Membaca suhu dari sensor DHT11.

Kemudian sistem memeriksa apakah data valid:

```cpp
if (isnan(suhu))
```

Jika pembacaan gagal, sistem menampilkan pesan:

```text
Gagal membaca data sensor!
```

Jika pembacaan berhasil, nilai suhu ditampilkan dan digunakan untuk menentukan kondisi relay.

---

## 3. Percabangan dan Logika Histerisis

### Kondisi 1 - Aktuator ON

```cpp
if (!aktuator && suhu > suhuThresholdHigh)
```

Kondisi ini berarti aktuator sedang OFF dan suhu melebihi 30°C.

Maka:

```cpp
aktuator = true;
digitalWrite(RELAYPIN, HIGH);
```

Aktuator berubah menjadi ON.

---

### Kondisi 2 - Aktuator OFF

```cpp
else if (aktuator && suhu < suhuThresholdLow)
```

Kondisi ini berarti aktuator sedang ON dan suhu turun di bawah 28°C.

Maka:

```cpp
aktuator = false;
digitalWrite(RELAYPIN, LOW);
```

Aktuator berubah menjadi OFF.

---

### Kondisi 3 - Mempertahankan Status

Jika suhu berada di antara 28°C dan 30°C, tidak ada perubahan status.

```text
28°C ≤ Suhu ≤ 30°C
```

Sistem mempertahankan kondisi aktuator sebelumnya.

Contohnya:

```text
Suhu 31°C → ON
Suhu 29°C → tetap ON
Suhu 28.5°C → tetap ON
Suhu 27°C → OFF
```

Hal tersebut merupakan prinsip histerisis.

---

## Alur Percobaan 2A

```text
Mulai
  ↓
Inisialisasi DHT11 dan Relay
  ↓
Baca suhu
  ↓
Data valid?
 ├── Tidak → Tampilkan pesan error
 └── Ya
       ↓
   Suhu > 30°C?
    ├── Ya → Aktuator ON
    └── Tidak
          ↓
      Suhu < 28°C?
       ├── Ya → Aktuator OFF
       └── Tidak → Pertahankan status
          ↓
      Tampilkan status
          ↓
       Delay 2 detik
          ↓
         Ulangi
```

---

## Dokumentasi
### Percobaan 1A
<img width="1280" height="960" alt="Percobaan 1a_IoT" src="https://github.com/user-attachments/assets/a2fe55a4-ffc8-47a6-beb5-d44a03b147ee" />
<img width="1280" height="960" alt="WhatsApp Image 2026-09-07 at 22 58 55" src="https://github.com/user-attachments/assets/bedc8166-5438-4297-8c87-29d8bc8d4b03" />

### Percobaan 2A
<img width="1280" height="960" alt="percobaan 2a_IoT" src="https://github.com/user-attachments/assets/111141f8-86f9-4158-93ac-b75a09ca8afc" />

