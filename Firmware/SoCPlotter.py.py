import serial
import time
import re
import matplotlib
matplotlib.use("TkAgg")

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# ================= SERIAL CONFIG =================
PORT = "COM16"          
BAUD = 115200          

try:
    ser = serial.Serial(PORT, BAUD, timeout=0.1)
    print("Serial connected on", PORT)
except Exception as e:
    print("Serial open failed:", e)
    exit(1)

# ================= DATA STORAGE =================
time_data = []
soc_data = []

start_time = time.time()
MAX_POINTS = 300   # rolling window size

# ================= PLOT SETUP =================
fig, ax = plt.subplots(figsize=(9, 5))

ax.set_title("Battery State of Charge (SoC) vs Time", fontsize=14, fontweight="bold")
ax.set_xlabel("Time (s)")
ax.set_ylabel("State of Charge (%)")
ax.set_ylim(80, 100)   # example: zoom into discharge region
ax.set_xlim(0, 300)     # show 60 seconds


ax.grid(True, linestyle="--", alpha=0.6)

line_soc, = ax.plot([], [], color="green", linewidth=2, label="SoC (%)")
ax.legend(loc="upper right")

# ================= UPDATE FUNCTION =================
def update(frame):
    try:
        raw = ser.readline()

        if not raw:
            return line_soc,

        # Decode safely (ignore binary garbage)
        line = raw.decode("ascii", errors="ignore").strip()

        # Ignore empty or non-printable lines
        if not line or not line.isprintable():
            return line_soc,

        # DEBUG: show what Python actually receives
        print("UART:", line)

        # Extract SoC using regex
        match = re.search(r'Battery\s+Charge\s+Percentage\s*:\s*(\d+)', line)
        if not match:
            return line_soc,

        soc_value = float(match.group(1))
        t = time.time() - start_time

        time_data.append(t)
        soc_data.append(soc_value)

        # Rolling window
        if len(time_data) > MAX_POINTS:
            time_data[:] = time_data[-MAX_POINTS:]
            soc_data[:] = soc_data[-MAX_POINTS:]

        line_soc.set_data(time_data, soc_data)

        # Move X-axis
        ax.set_xlim(max(0, t - 10), t + 0.5)

    except Exception as e:
        print("Parse error:", e)

    return line_soc,

# ================= ANIMATION =================
ani = FuncAnimation(
    fig,
    update,
    interval=100,              # 10 FPS (perfect for SoC)
    blit=False,
    cache_frame_data=False     # removes warning
)

# ================= START GUI =================
plt.tight_layout()
plt.show()

# ================= CLEANUP =================
ser.close()
