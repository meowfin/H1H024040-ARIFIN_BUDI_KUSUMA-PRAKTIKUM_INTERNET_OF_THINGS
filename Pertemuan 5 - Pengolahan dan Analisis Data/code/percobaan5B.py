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