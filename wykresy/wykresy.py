import pandas as pd
import matplotlib.pyplot as plt
from scipy.signal import firwin, lfilter
from datetime import datetime
import os

# ==========================
# Parametry filtra FIR
# ==========================
numtaps = 51
cutoff = 0.1

# ==========================
# Flagi wyświetlania
# ==========================
show_original_only = True
show_filtered_only = False
show_original_and_filtered = False

# ==========================
# Przedział czasu do wyświetlenia (ms)
# ==========================
time_start = 0
time_end = 1_000_000_0

# ==========================
# Ścieżka do pliku CSV
# ==========================
base_dir = os.path.dirname(os.path.abspath(__file__))
filename = os.path.join(base_dir, "data", "LOG_038.CSV")

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
def rysuj_wykres(x, y, nazwa, folder, mode="filtered", show_plot=False, is_binary=False):
    """
    mode:
        "original" - tylko oryginalne dane
        "filtered" - tylko po filtrze FIR
        "both"     - oryginalne + po filtrze FIR
    is_binary:
        True = kolumna binarna (0/1), bez FIR
    """
    if is_binary:
        plt.figure()
        plt.step(x, y, where='post')
        plt.title(nazwa)
        plt.xlabel("Time [ms]")
        plt.ylabel(nazwa)
        plt.grid(True)
        filename_plot = f"{nazwa}_binary.png"
        plt.savefig(os.path.join(folder, filename_plot))
        if show_plot:
            plt.show()
        plt.close()
        return

    # FIR dla kolumn numerycznych
    y_filt = lfilter(firwin(numtaps, cutoff), 1.0, y)

    plt.figure()
    nazwa = nazwa.split('[')[0].strip()

    if mode == "original":
        plt.plot(x, y, 'o', markersize=4)
        filename_plot = f"{nazwa}.png"
    elif mode == "filtered":
        plt.plot(x, y_filt, 'o', color='orange', markersize=4)
        filename_plot = f"{nazwa}_fir.png"
    elif mode == "both":
        plt.plot(x, y, 'o', markersize=4)
        plt.plot(x, y_filt, 'o-', markersize=4, linewidth=1)
        filename_plot = f"{nazwa}_original_vs_fir.png"

    plt.title(f"{nazwa}")
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

# Kolumny numeryczne
kolumny_num = [' Temperature1 [C]', ' Temperature2 [C]', ' Temperature3 [C]',
               ' Pressure1 [kPa]', ' Pressure2 [kPa]', ' Battery Voltage [V]']

# Kolumny binarne (Valve_State itp.)
columns_upper = [c.strip().upper() for c in df.columns]
kolumny_bin = [df.columns[i] for i, c in enumerate(columns_upper) if 'VALVE' in c or 'STATE' in c]

# ==========================
# Tworzenie wykresów
# ==========================
for kol in kolumny_num:
    if show_original_only:
        rysuj_wykres(x, df[kol], kol, output_folder, mode="original", show_plot=True)
    if show_filtered_only:
        rysuj_wykres(x, df[kol], kol, output_folder, mode="filtered", show_plot=True)
    if show_original_and_filtered:
        rysuj_wykres(x, df[kol], kol, output_folder, mode="both", show_plot=True)

for kol in kolumny_bin:
    rysuj_wykres(x, df[kol], kol, output_folder, is_binary=True, show_plot=True)

print("Wszystkie wykresy zapisane w folderze:", output_folder)
