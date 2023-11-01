# %%
import numpy as np
import matplotlib.pyplot as plt
from scipy import signal


# Define filter coefficients
b = [0.1, 0.2, 0.3, 0.4, 0.5]
a = [1, 0.5, 0.3, 0.2, 0.1]

# Calculate frequency response
w, h = signal.freqz(b, a)

# Plot frequency response
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.plot(w, 20 * np.log10(abs(h)))
ax1.set(
    title='Magnitude response',
    xlabel='Frequency (rad/sample)',
    ylabel='Magnitude (dB)')
ax1.grid()

ax2.plot(w, np.angle(h))
ax2.set(
    title='Phase response',
    xlabel='Frequency (rad/sample)',
    ylabel='Phase (radians)')
ax2.grid()

fig.tight_layout()

plt.show()

# %%
