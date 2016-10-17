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
            data = struct.unpack('h', file.read(2))
            r = data

            right.append(r)
            #left.append(l * 100)
        except struct.error as e:
            print(e)
            break

print(right)

plt.plot(right)
#plt.plot(left)
plt.show()
