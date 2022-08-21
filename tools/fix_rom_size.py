#!/usr/bin/python3
#
# Copyright (c) 2021, 2022 Adrian Siekierka
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
# CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

import os.path, struct, sys

size_file = os.path.getsize(sys.argv[1])
size_base = 32768
size_base_i = 0
while size_base < size_file:
	size_base *= 2
	size_base_i += 1

with open(sys.argv[1], "r+b") as fp:
	fp.seek(0x148)
	fp.write(struct.pack("<B", size_base_i))
	if (len(sys.argv) >= 3) and (sys.argv[2] == "tpp1"):
		fp.seek(0x147)
		fp.write(struct.pack("<B", 0xBC))
		fp.seek(0x149)
		fp.write(struct.pack("<B", 0xC1))
		fp.seek(0x14A)
		fp.write(struct.pack("<B", 0x65))
		fp.seek(0x150)
		fp.write(struct.pack("<B", 0x01))
		fp.seek(0x151)
		fp.write(struct.pack("<B", 0x00))
		fp.seek(0x152)
		fp.write(struct.pack("<B", int(sys.argv[3])))
		fp.seek(0x153)
		fp.write(struct.pack("<B", int(0x08)))
