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