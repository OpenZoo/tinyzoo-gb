#!/usr/bin/python3
import math, sys

APPLY_ZZT_ROUNDING = False
APPLY_PIT_ROUNDING = False

FREQS = [0] * 16 * 6

print("const uint16_t sound_freqs[%d] = {" % len(FREQS))

freqC1 = 32.0
ln2 = math.log(2)
noteStep = math.exp(ln2 / 12.0)

for octave in range(1, 6+1):
	noteBase = math.exp(octave * ln2) * freqC1
	noteBase = round(noteBase) # 64, 128, 256, 512, 1024, 2048
	for note in range(0, 12):
		if APPLY_ZZT_ROUNDING:
			FREQS[(octave - 1) * 16 + note] = int(noteBase)
		else:
			FREQS[(octave - 1) * 16 + note] = noteBase
		noteBase = noteBase * noteStep

print(FREQS, file=sys.stderr)

for i in FREQS:
	if i == 0:
		i = 0x0000
	else:
		if APPLY_PIT_ROUNDING:
			# PIT transformation
			i = math.floor(1193182.0 / i)
			# PIT de-transformation
			i = 1193182.0 / i
		i = round(2048 - (131072.0 / i)) | 0x8000
	print("\t%d," % i)

print("};")
