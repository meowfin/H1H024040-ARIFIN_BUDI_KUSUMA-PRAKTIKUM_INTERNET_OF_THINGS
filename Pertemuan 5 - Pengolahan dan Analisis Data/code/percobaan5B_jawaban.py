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