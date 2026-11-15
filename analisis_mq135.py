import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import pytz
import os

# --- KONFIGURASI ---
NAMA_FILE_CSV = 'feeds.csv'
NAMA_FILE_GRAFIK = 'analisis_mq135.png'
ZONA_WAKTU_LOKAL = pytz.timezone('Asia/Jakarta') # Sesuaikan dengan zona waktu demo kamu
JUDUL_UTAMA = 'Analisis Sensor Gas MQ-135 (5 - 15 November 2026)'
KOLOM_DATA = 'field1' # Field di ThingSpeak yang berisi data gas
WARNA_GARIS_TREN = '#1f77b4' # Biru standar matplotlib
WARNA_BAR_RATA2 = '#2ca02c' # Hijau standar matplotlib
WARNA_GARIS_MAX = '#d62728' # Merah standar matplotlib

# Pastikan file CSV ada
if not os.path.exists(NAMA_FILE_CSV):
    print(f"Error: File '{NAMA_FILE_CSV}' tidak ditemukan di folder ini.")
    exit()

print(f"Memproses file '{NAMA_FILE_CSV}'...")

# --- 1. MEMBACA DAN MENYIAPKAN DATA ---
# Membaca CSV
df = pd.read_csv(NAMA_FILE_CSV)

# Konversi kolom waktu ke datetime
df['created_at_dt'] = pd.to_datetime(df['created_at'])

# Konversi UTC ke Waktu Lokal (WIB)
df['created_at_lokal'] = df['created_at_dt'].dt.tz_convert(ZONA_WAKTU_LOKAL)

# Pastikan data numerik bersih (handle missing values jika ada)
df[KOLOM_DATA] = pd.to_numeric(df[KOLOM_DATA], errors='coerce')
df = df.dropna(subset=[KOLOM_DATA])

# --- 2. ANALISIS STATISTIK HARIAN ---
# Ekstrak tanggal lokal untuk pengelompokan
df['tanggal_lokal'] = df['created_at_lokal'].dt.date

# Hitung statistik harian
daily_stats = df.groupby('tanggal_lokal')[KOLOM_DATA].agg(
    ['count', 'mean', 'median', 'min', 'max', 'std']
).round(2)

print("\n=== STATISTIK HARIAN (WIB) ===")
print(daily_stats)
print("-" * 30)
print(f"Total data valid: {len(df)}")
print(f"Rentang Waktu: {df['created_at_lokal'].min()} hingga {df['created_at_lokal'].max()}")

# --- 3. MEMBUAT GRAFIK (VISUALISASI) ---
# Menggunakan gaya 'bmh' untuk grid yang rapi dan estetika profesional
plt.style.use('bmh')

# Membuat figure dengan 2 subplot vertikal
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8), sharex=False, gridspec_kw={'height_ratios': [1, 1]})

# --- Subplot 1: Tren Keseluruhan (Time-Series) ---
ax1.plot(df['created_at_lokal'], df['field1'], 
         color=WARNA_GARIS_TREN, 
         linewidth=1.0, 
         alpha=0.9, 
         label='Pembacaan Sensor')

# Styling Subplot 1
ax1.set_title('A. Tren Pembacaan Sensor Gas MQ-135 (Data Mentah)', fontsize=12, fontweight='bold')
ax1.set_ylabel('Nilai ADC (0 - 4095)', fontsize=10)
ax1.grid(True, which='both', linestyle='--', alpha=0.5)

# Format sumbu x (tanggal) agar rapi
ax1.xaxis.set_major_formatter(mdates.DateFormatter('%d %b', tz=ZONA_WAKTU_LOKAL))
ax1.tick_params(axis='x', rotation=45, labelsize=9)
ax1.legend(loc='upper right', fontsize=9)

# --- Subplot 2: Statistik Harian (Bar & Max Line) ---
tanggal_str = [str(d) for d in daily_stats.index]

# Bar chart untuk rata-rata harian
ax2.bar(tanggal_str, daily_stats['mean'], 
        color=WARNA_BAR_RATA2, 
        alpha=0.75, 
        label='Rata-rata Harian')

# Line plot untuk maksimum harian
ax2.plot(tanggal_str, daily_stats['max'], 
         color=WARNA_GARIS_MAX, 
         marker='o', 
         markersize=4, 
         linewidth=1.5, 
         label='Puncak (Maksimum) Harian')

# Styling Subplot 2
ax2.set_title('B. Rata-rata dan Puncak Harian Sensor Gas MQ-135', fontsize=12, fontweight='bold')
ax2.set_xlabel('Tanggal (WIB)', fontsize=10)
ax2.set_ylabel('Nilai ADC', fontsize=10)
ax2.set_xticks(tanggal_str) # Pastikan semua tanggal muncul di sumbu x
ax2.tick_params(axis='x', rotation=45, labelsize=9)
ax2.grid(True, which='major', linestyle='--', alpha=0.5)
ax2.legend(loc='upper right', fontsize=9)

# Tambahkan keterangan di bawah grafik
plt.figtext(0.5, 0.01, f'Sumber Data: {NAMA_FILE_CSV} | Total Data Poin: {len(df)} | Dihasilkan dengan Python Matplotlib', 
            ha='center', fontsize=8, style='italic', color='gray')

# Pengaturan layout agar tidak tumpang tindih
plt.tight_layout(rect=[0, 0.03, 1, 0.98])

# --- 4. MENYIMPAN GRAFIK ---
print(f"Menyimpan grafik sebagai '{NAMA_FILE_GRAFIK}'...")
plt.savefig(NAMA_FILE_GRAFIK, dpi=300, bbox_inches='tight')

# Tampilkan grafik di layar (opsional, jika menjalankan di environment GUI)
# plt.show()

print("Selesai.")