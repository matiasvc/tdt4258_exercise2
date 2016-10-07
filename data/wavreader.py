import wave, struct

waveFile = wave.open('corneria.wav', 'r')

lengt = waveFile.getnframes()

outputFile = open('data.txt', 'w')

for i in range(80000):
	waveData = waveFile.readframes(1)
	data = struct.unpack("<hh", waveData)
	dataString = "{}, {},\n".format(data[0], data[1])
	print(dataString)
	outputFile.write(dataString)


outputFile.close()
