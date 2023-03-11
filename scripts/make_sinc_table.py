import numpy as np
from scipy.special import sinc
from scipy.signal.windows import kaiser
import matplotlib.pyplot as plt

NZ = 32
SAMPLES_PER_CROSSING = 256
SINC_SIZE = NZ * SAMPLES_PER_CROSSING
KAISER_BETA = 10

x = np.linspace(-NZ, NZ, SINC_SIZE*2 + 1)
y = sinc(x)

window = kaiser(len(y), KAISER_BETA)

y = np.multiply(y ,window)

half_y = y[SINC_SIZE:]
half_x = x[SINC_SIZE:]

# plt.plot(x, y)
# plt.plot(half_x, half_y)
# plt.show()

print(f"// Auto-generated file from make_sinc_table.py")
print(f"// Number of zeros          : {NZ}")
print(f"// Samples per zero crossing: {SAMPLES_PER_CROSSING}")
print(f"// Kaiser window beta       : {KAISER_BETA}")
print("")
print(f"const uint32_t samples_per_crossing = {SAMPLES_PER_CROSSING};")
print("const float sinc_table[] = {")

for val in half_y:
    print(f"{val}f,")

print("};")