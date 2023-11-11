import numpy as np

TABLE_SIZE = 512

t = np.linspace(0, 2 * np.pi, TABLE_SIZE + 1)
x = np.sin(t)

print(f"// Auto-generated file from make_sine_table.py")
print("#pragma once")
print("")
print("namespace sfdsp")
print("{")
print(f"constexpr size_t SIN_LUT_SIZE = {TABLE_SIZE};")
print(f"const float sin_lut[TABLE_SIZE+1] = {{")

VAL_PER_LINE = 6
for i, val in enumerate(x):
    if i % VAL_PER_LINE == 0:
        print("")
    print(f"{val:.8f}f, ", end="")

print("};")
print("}")
