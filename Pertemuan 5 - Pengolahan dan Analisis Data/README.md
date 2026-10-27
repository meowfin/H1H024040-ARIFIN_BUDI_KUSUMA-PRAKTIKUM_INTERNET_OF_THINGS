# Modul 5: Pengolahan dan Analisis Data
## Library dan Dependencies
Library yang diperlukan pada praktikum:
### 5.1 Percobaan 5A

| Library/Komponen | Kegunaan |
|---|---|
| `math.h` | Menyediakan fungsi matematika seperti `sin()` dan `sqrt()`. |
| Serial Monitor | Menampilkan hasil pembacaan dan perhitungan. |
| DHT, opsional | Diperlukan jika program diubah untuk menggunakan sensor DHT22 fisik. |

### 5.2 Percobaan 5B

| Library | Kegunaan |
|---|---|
| `json` | Membaca payload MQTT dalam format JSON. |
| `time` | Mencatat waktu penerimaan data. |
| `paho-mqtt` | Menghubungkan Python dengan broker MQTT. |
| `pandas` | Mengelola data, CSV, dan perhitungan statistik. |
| `matplotlib` | Membuat grafik. |

Instalasi dependencies:

```bash
pip install paho-mqtt pandas matplotlib
```

`json` dan `time` merupakan library bawaan Python.

---
# Percobaan Praktikum
## Percobaan 5A
```cpp
// #include <DHT.h>             // Memanggil library untuk membaca sensor DHT 
// #define DHTPIN 4             // Menetapkan pin DATA sensor DHT pada GPIO 4 
// #define DHTTYPE DHT22        // Menentukan jenis sensor yang digunakan 
// DHT dht(DHTPIN, DHTTYPE);    // Membuat objek sensor DHT 
 
const int N = 5;                // Menetapkan ukuran jendela moving average sebanyak 5 data 
float buffer[N];                // Menyimpan data suhu yang digunakan untuk filter 
int indeks = 0;                 // Menentukan posisi penyimpanan data berikutnya 
int jumlah = 0;                 // Menghitung jumlah data yang telah tersimpan 
 
float suhuMin = 1000.0;         // Menginisialisasi suhu minimum dengan nilai awal yang tinggi 
float suhuMax = -1000.0;        // Menginisialisasi suhu maksimum dengan nilai awal yang rendah 
 
// Fungsi simulasi pembacaan suhu sebagai pengganti sensor DHT22 
float bacaSuhuMock() { 
    static unsigned long jumlahBaca = 0; // Menyimpan jumlah pembacaan meskipun fungsi selesai 
    jumlahBaca++;                        // Menambah penghitung setiap fungsi dipanggil 
 
    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08); 
    // Menghasilkan suhu simulasi yang berubah secara bertahap di sekitar 28°C 
 
    float noise = random(-5, 6) * 0.1; 
    // Menghasilkan gangguan acak dari -0,5°C sampai 0,5°C 
 
    float suhu = suhuDasar + noise;      // Menambahkan gangguan ke suhu dasar 
 
    if (jumlahBaca % 25 == 0) {          // Memeriksa setiap kelipatan 25 pembacaan 
        suhu += 8.0;                     // Menambahkan lonjakan 8°C sebagai simulasi anomali 
    } 
 
    return suhu;                         // Mengembalikan nilai suhu simulasi 
} 
 
// Fungsi untuk menghitung rata-rata bergerak 
float hitungMovingAverage(float nilaiBaru) { 
    buffer[indeks] = nilaiBaru;          // Menyimpan data baru ke dalam buffer 
    indeks = (indeks + 1) % N;           // Memajukan indeks dan kembali ke 0 setelah mencapai N 
    if (jumlah < N) jumlah++;             // Menambah jumlah data hingga buffer terisi penuh 
 
    float total = 0;                     // Menyiapkan variabel penjumlahan 
    for (int i = 0; i < jumlah; i++) {    // Mengulangi seluruh data yang telah terisi 
        total += buffer[i];              // Menambahkan setiap data ke total 
    } 
 
    return total / jumlah;               // Mengembalikan rata-rata data dalam buffer 
} 
 
void setup() { 
    Serial.begin(115200);                // Mengaktifkan komunikasi serial 115200 baud 
    // dht.begin();                       // Menginisialisasi sensor jika memakai DHT22 fisik 
} 
 
void loop() { 
    // float suhu = dht.readTemperature(); // Membaca suhu dari sensor DHT22 
    float suhu = bacaSuhuMock();          // Mengambil suhu dari fungsi simulasi 
 
    if (isnan(suhu)) {                    // Memeriksa apakah data bukan angka yang valid 
        Serial.println("Pembacaan gagal, data diabaikan"); 
        // Menampilkan pesan jika pembacaan tidak valid 
    } else { 
        float suhuFilter = hitungMovingAverage(suhu); 
        // Menghitung nilai suhu hasil filter moving average 
 
        if (suhu < suhuMin) suhuMin = suhu; 
        // Memperbarui suhu minimum jika ditemukan nilai yang lebih rendah 
 
        if (suhu > suhuMax) suhuMax = suhu; 
        // Memperbarui suhu maksimum jika ditemukan nilai yang lebih tinggi 
 
        Serial.print("Mentah: ");        // Menampilkan label suhu mentah 
        Serial.print(suhu);              // Menampilkan nilai suhu sebelum filtering 
        Serial.print(" | Filter: ");     // Menampilkan label hasil filter 
        Serial.print(suhuFilter);        // Menampilkan nilai suhu setelah filtering 
        Serial.print(" | Min: ");        // Menampilkan label suhu minimum 
        Serial.print(suhuMin);           // Menampilkan suhu minimum sejak program berjalan 
        Serial.print(" | Max: ");        // Menampilkan label suhu maksimum 
        Serial.println(suhuMax);         // Menampilkan suhu maksimum dan mengakhiri baris 
    } 
 
delay(2000);                         
// Menunggu 2 detik sebelum pembacaan berikutnya 
}
```
## Alur Percobaan 5A:

```text
          ESP32
            |
            v
      bacaSuhuMock()
            |
            v
        Data suhu
            |
            v
     Moving average
            |
            v
      Simpangan baku
            |
            v
   Minimum dan maksimum
            |
            v
      Serial Monitor
```

---

## Percobaan 5B
```python
import json #// Mengimpor pustaka json untuk memproses data berformat JSON
import time #// Mengimpor pustaka time untuk mengambil waktu sistem saat ini
import paho.mqtt.client as mqtt #// Mengimpor pustaka Paho MQTT untuk komunikasi protokol MQTT
import pandas as pd #// Mengimpor pustaka pandas untuk manipulasi tabel data dan penyimpanan CSV
import matplotlib.pyplot as plt #// Mengimpor pustaka matplotlib untuk membuat grafik visualisasi data

BROKER = "broker.hivemq.com" #// Menentukan alamat server broker MQTT publik yang digunakan
PORT = 1883 #// Menentukan nomor port standar untuk koneksi MQTT tanpa enkripsi
TOPIC = "ahlele/ahlelas/dhtPalsu" #// Menentukan topik MQTT tempat menerima data sensor dari ESP32
JUMLAH_SAMPEL = 30 #// Menentukan batas maksimal jumlah sampel data suhu yang akan diambil

data = [] #// Membuat list kosong untuk menampung data waktu dan suhu yang diterima

def on_connect(client, userdata, flags, reason_code, properties): #// Mendefinisikan fungsi callback yang dijalankan otomatis saat berhasil terhubung ke broker
    print("Terhubung ke broker, kode:", reason_code) #// Mencetak pesan status koneksi beserta kode hasilnya ke layar
    client.subscribe(TOPIC) #// Berlangganan (subscribe) ke topik yang telah ditentukan agar dapat menerima pesan

def on_message(client, userdata, msg): #// Mendefinisikan fungsi callback yang dijalankan otomatis setiap kali ada pesan masuk dari topik
    try: #// Memulai blok percobaan untuk menangkap potensi error saat membaca data
        payload = json.loads(msg.payload.decode()) #// Mengubah isi pesan dari format byte menjadi string, lalu menguraikannya sebagai objek JSON
        suhu = float(payload["suhu"]) #// Mengambil nilai dari kunci "suhu" pada JSON dan mengubah tipenya menjadi angka desimal (float)
    except (ValueError, KeyError): #// Menangkap error jika nilai bukan angka yang valid (ValueError) atau kunci "suhu" tidak ditemukan (KeyError)
        print("Data tidak valid, diabaikan") #// Mencetak pesan peringatan bahwa data yang masuk rusak atau tidak sesuai format
        return #// Menghentikan eksekusi fungsi lebih awal agar data yang tidak valid tidak disimpan

    data.append({"waktu": time.strftime("%Y-%m-%d %H:%M:%S"), "suhu": suhu}) #// Menambahkan dictionary berisi waktu saat ini dan nilai suhu ke dalam list data
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}") #// Menampilkan progres jumlah data yang terkumpul beserta nilai suhu terbaru ke layar

    if len(data) >= JUMLAH_SAMPEL: #// Memeriksa apakah jumlah data yang terkumpul sudah mencapai batas sampel (30 data)
        client.disconnect() #// Memutuskan koneksi dari broker MQTT jika jumlah sampel sudah terpenuhi

# --- Tahap 1: akuisisi data dari MQTT --- #// Komentar penanda dimulainya tahap pertama yaitu pengambilan data melalui MQTT
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2) #// Membuat objek client MQTT baru menggunakan API callback versi 2
client.on_connect = on_connect #// Menghubungkan fungsi on_connect ke event saat client tersambung ke broker
client.on_message = on_message #// Menghubungkan fungsi on_message ke event saat client menerima pesan baru
client.connect(BROKER, PORT) #// Melakukan koneksi ke alamat broker dan port yang sudah ditentukan
client.loop_forever() #// Menjalankan perulangan jaringan terus-menerus untuk memproses pesan hingga koneksi diputus

# --- Tahap 2: simpan ke CSV --- #// Komentar penanda dimulainya tahap kedua yaitu penyimpanan data ke file CSV
df = pd.DataFrame(data) #// Mengonversi list data yang terkumpul menjadi struktur tabel DataFrame pandas
df.to_csv("data_suhu.csv", index=False) #// Menyimpan tabel DataFrame ke file bernama "data_suhu.csv" tanpa menyertakan kolom nomor indeks
print("Data tersimpan di data_suhu.csv") #// Mencetak pesan konfirmasi bahwa file CSV mentah berhasil disimpan

# --- Tahap 3: analisis statistik dan deteksi anomali (z-score) --- #// Komentar penanda dimulainya tahap ketiga yaitu perhitungan statistik dan deteksi anomali
rata2 = df["suhu"].mean() #// Menghitung nilai rata-rata (mean) dari seluruh data pada kolom "suhu"
std = df["suhu"].std() #// Menghitung nilai simpangan baku (standar deviasi) dari kolom "suhu"

if std == 0: #// Memeriksa apakah standar deviasi bernilai nol (semua data suhu bernilai sama persis)
    df["zscore"] = 0.0 #// Mengatur seluruh nilai kolom "zscore" menjadi 0.0 untuk menghindari error pembagian dengan nol
else: #// Jika standar deviasi tidak nol (data suhu bervariasi)
    df["zscore"] = (df["suhu"] - rata2) / std #// Menghitung skor-Z untuk setiap baris dengan rumus (nilai suhu dikurangi rata-rata) dibagi standar deviasi

df["anomali"] = df["zscore"].abs() > 2 #// Membuat kolom boolean "anomali" yang bernilai True jika nilai mutlak z-score lebih dari 2

print(f"Rata-rata : {rata2:.2f}") #// Mencetak nilai rata-rata suhu dengan pembulatan dua angka di belakang koma
print(f"Std deviasi: {std:.2f}") #// Mencetak nilai standar deviasi dengan pembulatan dua angka di belakang koma
print(f"Minimum : {df['suhu'].min():.2f}") #// Mencetak nilai suhu terendah dengan pembulatan dua angka di belakang koma
print(f"Maksimum : {df['suhu'].max():.2f}") #// Mencetak nilai suhu tertinggi dengan pembulatan dua angka di belakang koma
print(f"Jumlah anomali: {df['anomali'].sum()}") #// Mencetak total jumlah data yang terdeteksi sebagai anomali (menjumlahkan nilai True)

# --- Tahap 4: visualisasi --- #// Komentar penanda dimulainya tahap keempat yaitu pembuatan grafik visualisasi
plt.figure(figsize=(10, 4)) #// Membuat kanvas grafik baru dengan ukuran lebar 10 inci dan tinggi 4 inci
plt.plot(df.index, df["suhu"], marker="o", label="Suhu") #// Menggambar grafik garis suhu terhadap urutan indeks dengan titik penanda berbentuk lingkaran
anom = df[df["anomali"]] #// Menyaring dan mengambil hanya baris-baris data yang terdeteksi sebagai anomali (bernilai True)
plt.scatter(anom.index, anom["suhu"], color="red", s=100, label="Anomali", zorder=3) #// Menambahkan titik merah berukuran besar pada posisi data anomali di lapisan atas (zorder=3)
plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata") #// Menggambar garis horizontal putus-putus berwarna abu-abu pada posisi nilai rata-rata suhu
plt.xlabel("Urutan data") #// Memberikan label teks "Urutan data" pada sumbu mendatar (X)
plt.ylabel("Suhu (°C)") #// Memberikan label teks "Suhu (°C)" pada sumbu tegak (Y)
plt.title("Data Suhu dan Deteksi Anomali") #// Memberikan judul utama pada bagian atas grafik
print(df.round(2).to_string()) #// Menampilkan seluruh isi tabel DataFrame ke layar dengan angka desimal dibulatkan 2 digit
df.to_csv("data_suhu_analisis.csv", index=False) #// Menyimpan tabel DataFrame yang sudah dilengkapi hasil analisis ke file "data_suhu_analisis.csv"
plt.legend() #// Menampilkan kotak keterangan (legenda) untuk menjelaskan arti garis dan titik pada grafik
plt.tight_layout() #// Merapikan tata letak elemen grafik secara otomatis agar tidak ada teks yang terpotong
plt.savefig("grafik_suhu.png") #// Menyimpan hasil grafik ke dalam file gambar bernama "grafik_suhu.png"
plt.show() #// Menampilkan jendela grafik secara langsung ke layar
```

## Alur Percobaan 5B:

```text
ESP32 (publisher)
       |
       v
Broker MQTT HiveMQ
       |
       v
Python (subscriber)
       |
       v
Penyimpanan CSV
       |
       v
Analisis data mentah
       |
       v
Moving average (5)
       |
       v
Z-score dan deteksi anomali
       |
       v
Perbandingan hasil dan grafik
```
---

# Pertanyaan Praktikum
## Modifikasi Percobaan 5A:
```cpp
#include <math.h>

// Konfigurasi sensor DHT22 jika digunakan secara fisik.
// #include <DHT.h>
// #define DHTPIN 4
// #define DHTTYPE DHT22
// DHT dht(DHTPIN, DHTTYPE);

const int N = 5;         // Jumlah sampel pada buffer.
float buffer[N];         // Menyimpan sampel suhu terakhir.
int indeks = 0;          // Posisi penulisan berikutnya.
int jumlah = 0;          // Jumlah sampel yang sudah tersimpan.

float suhuMin = 1000.0;  // Inisialisasi suhu minimum.
float suhuMax = -1000.0; // Inisialisasi suhu maksimum.

// Menghasilkan data suhu simulasi.
float bacaSuhuMock() {
    static unsigned long jumlahBaca = 0;
    jumlahBaca++;

    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);
    float noise = random(-5, 6) * 0.1;
    float suhu = suhuDasar + noise;

    // Simulasi lonjakan suhu setiap 25 pembacaan.
    if (jumlahBaca % 25 == 0) {
        suhu += 8.0;
    }

    return suhu;
}

// Menghitung moving average dari data pada buffer.
float hitungMovingAverage(float nilaiBaru) {
    buffer[indeks] = nilaiBaru;
    indeks = (indeks + 1) % N;

    if (jumlah < N) {
        jumlah++;
    }

    float total = 0;

    for (int i = 0; i < jumlah; i++) {
        total += buffer[i];
    }

    return total / jumlah;
}

// [TAMBAHAN] Menghitung simpangan baku populasi pada buffer.
float hitungSimpanganBaku() {
    // [TAMBAHAN] Menghindari pembagian dengan nol.
    if (jumlah == 0) {
        return 0.0;
    }

    // [TAMBAHAN] Menghitung jumlah seluruh sampel.
    float total = 0.0;

    for (int i = 0; i < jumlah; i++) {
        total += buffer[i];
    }

    // [TAMBAHAN] Menghitung rata-rata sampel pada buffer.
    float rataRata = total / jumlah;

    // [TAMBAHAN] Menampung jumlah kuadrat selisih.
    float totalKuadrat = 0.0;

    for (int i = 0; i < jumlah; i++) {
        float selisih = buffer[i] - rataRata;
        totalKuadrat += selisih * selisih;
    }

    // [TAMBAHAN] Menghitung varians populasi.
    float varians = totalKuadrat / jumlah;

    // [TAMBAHAN] Akar varians menghasilkan simpangan baku.
    return sqrt(varians);
}

void setup() {
    Serial.begin(115200);

    // Aktifkan jika menggunakan sensor DHT22 fisik.
    // dht.begin();
}

void loop() {
    // Gunakan bacaSuhuMock() untuk simulasi.
    // Gunakan dht.readTemperature() jika DHT22 aktif.
    float suhu = bacaSuhuMock();

    if (isnan(suhu)) {
        Serial.println("Pembacaan gagal, data diabaikan");
    } else {
        float suhuFilter = hitungMovingAverage(suhu);

        // [TAMBAHAN] Menghitung simpangan baku buffer saat ini.
        float simpanganBaku = hitungSimpanganBaku();

        if (suhu < suhuMin) {
            suhuMin = suhu;
        }

        if (suhu > suhuMax) {
            suhuMax = suhu;
        }

        Serial.print("Mentah: ");
        Serial.print(suhu);

        Serial.print(" | Filter: ");
        Serial.print(suhuFilter);

        Serial.print(" | Simpangan Baku: ");

        // [TAMBAHAN] Menampilkan simpangan baku pada Serial Monitor.
        Serial.print(simpanganBaku);

        Serial.print(" | Min: ");
        Serial.print(suhuMin);

        Serial.print(" | Max: ");
        Serial.println(suhuMax);
    }

    delay(2000);
}
```
## Penjelasan program modifikasi percobaan 5A

### Penjelasan fungsi

**1. `bacaSuhuMock()`**  
Menghasilkan suhu simulasi menggunakan fungsi sinus, noise acak kecil, dan lonjakan suhu setiap 25 pembacaan. Variabel `jumlahBaca` menggunakan `static` agar nilainya tidak kembali ke nol setiap fungsi dipanggil.

**2. `hitungMovingAverage(float nilaiBaru)`**  
Memasukkan nilai baru ke buffer, memperbarui indeks, lalu menghitung rata-rata seluruh sampel yang tersedia.

**3. `hitungSimpanganBaku()`**  
Menghitung rata-rata sampel, menjumlahkan kuadrat selisih terhadap rata-rata, menghitung varians populasi, kemudian mengambil akar kuadratnya. Fungsi mengembalikan `0.0` jika buffer belum memiliki sampel.

**4. `setup()`**  
Dijalankan satu kali ketika ESP32 menyala. `Serial.begin(115200)` memulai komunikasi serial.

**5. `loop()`**  
Dijalankan berulang kali untuk mengambil suhu, memeriksa validitasnya, menghitung *moving average* dan simpangan baku, memperbarui minimum dan maksimum, serta menampilkan hasil ke Serial Monitor.

### Penjelasan Percabangan dan Perulangan

| Kondisi/perulangan | Kegunaan |
|---|---|
| `if (jumlahBaca % 25 == 0)` | Menambahkan lonjakan suhu setiap 25 pembacaan. |
| `if (jumlah < N)` | Menjaga jumlah sampel agar tidak melebihi kapasitas buffer. |
| `if (jumlah == 0)` | Menghindari pembagian dengan nol. |
| `if (isnan(suhu))` | Memeriksa nilai suhu yang tidak valid. |
| `if (suhu < suhuMin)` | Memperbarui suhu minimum. |
| `if (suhu > suhuMax)` | Memperbarui suhu maksimum. |
| `for` | Mengulang perhitungan untuk setiap sampel di buffer. |

---

## Modifikasi Percobaan 5B
```python
import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "ahlele/ahlelas/dhtPalsu"
JUMLAH_SAMPEL = 30

data = []

def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        suhu = float(payload["suhu"])
    except (ValueError, KeyError, TypeError, json.JSONDecodeError):
        print("Data tidak valid, diabaikan")
        return

    data.append({
        "waktu": time.strftime("%Y-%m-%d %H:%M:%S"),
        "suhu": suhu
    })

    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")

    if len(data) >= JUMLAH_SAMPEL:
        client.disconnect()

# Tahap 1: Akuisisi data dari MQTT
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT)
client.loop_forever()

# Tahap 2: Simpan data mentah ke CSV
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data tersimpan di data_suhu.csv")

# Tahap 3: Analisis data mentah
rata2_raw = df["suhu"].mean()
std_raw = df["suhu"].std()

# Tambahan 1: Hitung Z-score data mentah.
# Jika standar deviasi nol, semua Z-score diatur menjadi nol.
if std_raw == 0 or pd.isna(std_raw):
    df["zscore_raw"] = 0.0
else:
    df["zscore_raw"] = (df["suhu"] - rata2_raw) / std_raw

# Tambahan 2: Tandai anomali data mentah.
df["anomali_raw"] = df["zscore_raw"].abs() > 2

# Tambahan 3: Terapkan moving average dengan jendela 5 data.
# min_periods=1 memungkinkan perhitungan dimulai dari sampel pertama.
df["suhu_ma"] = df["suhu"].rolling(
    window=5,
    min_periods=1
).mean()

# Tambahan 4: Hitung rata-rata data yang telah difilter.
rata2_ma = df["suhu_ma"].mean()

# Tambahan 5: Hitung standar deviasi data hasil moving average.
std_ma = df["suhu_ma"].std()

# Tambahan 6: Hitung Z-score setelah moving average.
if std_ma == 0 or pd.isna(std_ma):
    df["zscore_ma"] = 0.0
else:
    df["zscore_ma"] = (df["suhu_ma"] - rata2_ma) / std_ma

# Tambahan 7: Tandai anomali pada data hasil moving average.
df["anomali_ma"] = df["zscore_ma"].abs() > 2

# Tahap 4: Tampilkan statistik dan perbandingan.
print(f"Rata-rata suhu mentah: {rata2_raw:.2f}")
print(f"Standar deviasi mentah: {std_raw:.2f}")
print(f"Minimum suhu mentah: {df['suhu'].min():.2f}")
print(f"Maksimum suhu mentah: {df['suhu'].max():.2f}")

print(f"Jumlah anomali data mentah: {df['anomali_raw'].sum()}")
print(f"Jumlah anomali setelah moving average: {df['anomali_ma'].sum()}")

# Tahap 5: Simpan hasil analisis ke CSV.
df.to_csv("data_suhu_analisis.csv", index=False)
print("Hasil analisis tersimpan di data_suhu_analisis.csv")

print(df.round(2).to_string())

# Tahap 6: Visualisasi data.
plt.figure(figsize=(12, 5))

# Menampilkan data suhu mentah.
plt.plot(
    df.index,
    df["suhu"],
    marker="o",
    label="Suhu Mentah"
)

# Menampilkan hasil moving average.
plt.plot(
    df.index,
    df["suhu_ma"],
    linestyle="--",
    label="Moving Average (5)"
)

# Menampilkan anomali pada data mentah.
anom_raw = df[df["anomali_raw"]]
plt.scatter(
    anom_raw.index,
    anom_raw["suhu"],
    color="red",
    s=80,
    label="Anomali Data Mentah",
    zorder=3
)

# Menampilkan anomali setelah moving average.
anom_ma = df[df["anomali_ma"]]
plt.scatter(
    anom_ma.index,
    anom_ma["suhu_ma"],
    color="purple",
    marker="x",
    s=100,
    label="Anomali Setelah Filtering",
    zorder=4
)

plt.axhline(
    rata2_raw,
    color="gray",
    linestyle=":",
    label="Rata-rata Data Mentah"
)

plt.xlabel("Urutan Data")
plt.ylabel("Suhu (°C)")
plt.title("Perbandingan Suhu Mentah, Moving Average, dan Anomali")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu.png")
plt.show()
```

## Penjelasan program modifikasi percobaan 5B

**1. `on_connect()`**  

Dijalankan ketika klien Python terhubung ke broker. Fungsi ini menampilkan kode koneksi dan berlangganan ke topik yang ditentukan.

**2. `on_message()`**  

Dijalankan ketika pesan MQTT diterima. Payload diubah dari byte menjadi JSON, kemudian nilai suhu dikonversi menjadi `float`. Data valid disimpan bersama waktu penerimaan. Setelah jumlah sampel mencapai 30, klien memutuskan koneksi.

**3. Penyimpanan CSV**  

`pd.DataFrame(data)` mengubah data menjadi tabel, sedangkan `to_csv()` menyimpan data ke file CSV.

**4. Analisis Z-score**  

Rata-rata dan simpangan baku dihitung dari data suhu. Setiap nilai dibandingkan dengan rata-rata, kemudian ditandai sebagai anomali jika nilai absolut Z-score lebih besar dari 2.

**5. Moving average**

`rolling(window=5, min_periods=1).mean()` menghitung rata-rata bergerak dari lima sampel terakhir, atau dari sampel yang tersedia pada awal data.

**6. Menghitung Z-score data mentah**

```python
rata2_raw = df["suhu"].mean()
std_raw = df["suhu"].std()
```

Menghitung rata-rata dan simpangan baku suhu mentah sebagai dasar Z-score awal.

**6. Menentukan anomali data mentah**

```python
df["anomali_raw"] = df["zscore_raw"].abs() > 2
```

Menghasilkan status `True` jika nilai absolut Z-score melebihi 2.

**7. Menerapkan moving average berjendela lima**

```python
df["suhu_ma"] = df["suhu"].rolling(
    window=5,
    min_periods=1
).mean()
```

`window=5` menentukan jumlah sampel dalam jendela. `min_periods=1` mengizinkan perhitungan pada awal data ketika jumlah sampel belum mencapai lima.

**8. Menghitung statistik data terfilter**

```python
rata2_ma = df["suhu_ma"].mean()
std_ma = df["suhu_ma"].std()
```

Menghitung rata-rata dan simpangan baku dari data yang sudah difilter.

**9. Menghitung Z-score data terfilter**

```python
if std_ma == 0 or pd.isna(std_ma):
    df["zscore_ma"] = 0.0
else:
    df["zscore_ma"] = (df["suhu_ma"] - rata2_ma) / std_ma
```

Menangani simpangan baku nol atau tidak tersedia, kemudian menghitung Z-score jika perhitungan memungkinkan.

**10. Menandai anomali setelah filtering**

```python
df["anomali_ma"] = df["zscore_ma"].abs() > 2
```

Menandai data hasil filter sebagai anomali jika nilai absolut Z-score lebih besar dari 2.

**11. Membandingkan jumlah anomali**

```python
print(f"Jumlah anomali data mentah: {df['anomali_raw'].sum()}")
print(f"Jumlah anomali setelah moving average: {df['anomali_ma'].sum()}")
```

Fungsi `sum()` menghitung jumlah nilai `True` pada kolom Boolean. Hasil ini digunakan untuk membandingkan kedua metode berdasarkan data aktual.

**12. Menyimpan hasil analisis**

```python
df.to_csv("data_suhu_analisis.csv", index=False)
```

Menyimpan suhu mentah, hasil filter, Z-score, dan status anomali ke file CSV.

**13. Membuat grafik perbandingan**

```python
plt.plot(df.index, df["suhu"], marker="o", label="Suhu Mentah")
plt.plot(df.index, df["suhu_ma"], linestyle="--", label="Moving Average (5)")
```

Menampilkan suhu mentah dan hasil *moving average* dalam satu grafik agar perubahan pola lebih mudah dibandingkan.

---

## Dokumentasi
### Alat dan Bahan
<img width="960" height="1280" alt="alat dan bahan percobaan modul 5" src="https://github.com/user-attachments/assets/c86a9004-b7f5-45dc-85e5-b001d9a472d0" />

### Percobaan 5A
<img width="1280" height="960" alt="percobaan 5A_modul 5" src="https://github.com/user-attachments/assets/4b998b63-eac1-46c7-bcf9-4659c43f53f6" />

### Percobaan 5B
<img width="1280" height="960" alt="percobaan 5B_modul 5" src="https://github.com/user-attachments/assets/38f3d7c0-428e-4bc1-a8fe-b7b597065e5d" />

### Percobaan 5B anomali
<img width="1000" height="400" alt="percobaan 5B_modul 5  anomali" src="https://github.com/user-attachments/assets/2d576f61-dfbb-4be1-9b0a-0c7229c26150" />

---

