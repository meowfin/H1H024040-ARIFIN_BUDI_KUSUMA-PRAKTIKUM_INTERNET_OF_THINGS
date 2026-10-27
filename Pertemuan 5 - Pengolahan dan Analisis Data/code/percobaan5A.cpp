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