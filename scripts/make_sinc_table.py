import numpy as np
from scipy.special import sinc
from scipy.signal.windows import kaiser
import matplotlib.pyplot as plt

NZ = 32
SAMPLES_PER_CROSSING = 512
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
print("#pragma once")
print("")
print(f"constexpr size_t SINC_ZERO_COUNT = {NZ};")
print(f"constexpr size_t SAMPLES_PER_CROSSING = {SAMPLES_PER_CROSSING};")
print(f"constexpr size_t sinc_table_size = {SINC_SIZE};")
print(f"const float sinc_table[{SINC_SIZE+1}] = {{")

for val in half_y:
    print(f"{val}f,")

print("};")