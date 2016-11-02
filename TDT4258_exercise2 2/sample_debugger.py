import struct
import matplotlib.pyplot as plt

# Use:
# dump binary memory samples 0x200010b4 0x20002ff4
# in gdb to dump memory

right = []
left = []

with open('samples', 'rb') as file:
    while True:
        try:
            data = struct.unpack('hh', file.read(4))
            r, l = data

            right.append(r)
            left.append(l)
        except struct.error as e:
            print(e)
            break

print(right)

plt.plot(right)
plt.plot(left)
plt.show()
