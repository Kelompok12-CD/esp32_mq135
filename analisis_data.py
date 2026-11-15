import pandas as pd
import matplotlib.pyplot as plt

# 1. Membaca dataset mentah dari ThingSpeak
df = pd.read_csv('feeds.csv')
df['created_at_wib'] = pd.to_datetime(df['created_at']).dt.tz_convert('Asia/Jakarta')
df['date_wib'] = df['created_at_wib'].dt.date

# 2. Menghitung statistik harian (untuk tabel laporan)
daily_stats = df.groupby('date_wib')['field1'].agg(['count', 'mean', 'median', 'min', 'max', 'std']).round(2)
print("=== STATISTIK HARIAN SENSOR MQ-135 (WIB) ===")
print(daily_stats)

# 3. Membuat grafik tren & rata-rata harian
plt.figure(figsize=(12, 6))

plt.subplot(2, 1, 1)
plt.plot(df['created_at_wib'], df['field1'], linewidth=0.6, color='#1f77b4')
plt.title('Tren Pembacaan Sensor Gas MQ-135 (5 - 15 November 2026, WIB)')
plt.ylabel('Nilai ADC (0 - 4095)')
plt.grid(True, linestyle='--', alpha=0.5)

plt.subplot(2, 1, 2)
daily_mean = df.groupby('date_wib')['field1'].mean()
daily_max = df.groupby('date_wib')['field1'].max()
dates_str = [str(d) for d in daily_mean.index]
plt.bar(dates_str, daily_mean.values, color='#2ca02c', alpha=0.8, label='Rata-rata Harian')
plt.plot(dates_str, daily_max.values, color='#d62728', marker='o', label='Maksimum Harian')
plt.title('Rata-rata dan Puncak Harian Sensor Gas MQ-135')
plt.ylabel('Nilai ADC')
plt.xticks(rotation=45)
plt.legend()
plt.grid(True, linestyle='--', alpha=0.5)

plt.tight_layout()
plt.savefig('analisis_mq135.png', dpi=150)
print("Grafik berhasil disimpan sebagai analisis_mq135.png")