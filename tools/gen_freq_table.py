#!/usr/bin/python3
#
# Copyright (c) 2020, 2021, 2022 Adrian Siekierka
#
# Based on a reconstruction of code from ZZT,
# Copyright 1991 Epic MegaGames, used with permission.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

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
