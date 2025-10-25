import pandas as pd
import matplotlib.pyplot as plt
from scipy.signal import firwin, lfilter
from datetime import datetime
import os

# ==========================
# Parametry filtra FIR
# ==========================
numtaps = 51      # liczba współczynników filtra
cutoff = 0.1      # częstotliwość odcięcia (0 < cutoff < 0.5, gdzie 0.5 = Nyquist)

# ==========================
# Flagi wyświetlania
# ==========================
show_original_only = False      # True = wyświetla tylko oryginalne dane
show_filtered_only = True     # True = wyświetla tylko po filtrze FIR
show_original_and_filtered = False  # True = wyświetla oryginalne + po filtrze FIR

# ==========================
# Przedział czasu do wyświetlenia (ms)
# ==========================
time_start = 0    # początek przedziału w ms
time_end = 1_000_000_0      # koniec przedziału w ms

# ==========================
# Ścieżka do pliku CSV
# ==========================
base_dir = os.path.dirname(os.path.abspath(__file__))
filename = os.path.join(base_dir, "data", "LOG_020.CSV")

df = pd.read_csv(filename)

# ==========================
# Filtrowanie danych wg przedziału czasu
# ==========================
df = df[(df['Time [ms]'] >= time_start) & (df['Time [ms]'] <= time_end)]

# ==========================
# Folder na wykresy
# ==========================
today_str = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
output_folder = os.path.join(base_dir, today_str)
os.makedirs(output_folder, exist_ok=True)

# ==========================
# Funkcja do tworzenia wykresów
# ==========================
def rysuj_wykres(x, y, nazwa, folder, mode="filtered", show_plot=False):
    """
    mode:
        "original" - tylko oryginalne dane
        "filtered" - tylko po filtrze FIR
        "both"     - oryginalne + po filtrze FIR
    """
    y_filt = lfilter(firwin(numtaps, cutoff), 1.0, y)

    plt.figure()
    nazwa = nazwa.split('[')[0].strip()  
    
    if mode == "original":
        plt.plot(x, y, 'o', markersize=4)
        plt.title(f"{nazwa}")
        filename_plot = f"{nazwa}.png"

    elif mode == "filtered":
        plt.plot(x, y_filt, 'o', color='orange', markersize=4)
        plt.title(f"{nazwa}")
        filename_plot = f"{nazwa}_fir.png"

    elif mode == "both":
        plt.plot(x, y, 'o', markersize=4)
        plt.plot(x, y_filt, 'o-', markersize=4, linewidth=1)
        plt.title(f"{nazwa} - Oryginalne vs Po filtrze FIR")
        filename_plot = f"{nazwa}_original_vs_fir.png"
    
    plt.xlabel("Time [ms]")
    plt.ylabel(nazwa)
    plt.legend()
    plt.grid(True)
    plt.savefig(os.path.join(folder, filename_plot))
    
    if show_plot:
        plt.show()
    
    plt.close()

# ==========================
# Dane X i kolumny Y
# ==========================
x = df['Time [ms]']
kolumny = [' Temperature1 [C]',' Temperature2 [C]', ' Temperature3 [C]',
           ' Pressure1 [kPa]', ' Pressure2 [kPa]', ' Battery Voltage [mV]']

# ==========================
# Tworzenie wykresów w zależności od flag
# ==========================
for kol in kolumny:
    if show_original_only:
        rysuj_wykres(x, df[kol], kol, output_folder, mode="original", show_plot=True)
    if show_filtered_only:
        rysuj_wykres(x, df[kol], kol, output_folder, mode="filtered", show_plot=True)
    if show_original_and_filtered:
        rysuj_wykres(x, df[kol], kol, output_folder, mode="both", show_plot=True)

print("Wszystkie wykresy zapisane w folderze:", output_folder)
