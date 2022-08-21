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

import os, random, sys

r_seed = 42
if len(sys.argv) > 4:
	r_seed = int(sys.argv[4])
random.seed(r_seed)

t_padding = 4

with open(sys.argv[1], "w") as fp:
	width = int(sys.argv[2])
	height = int(sys.argv[3])
	entry_count = width * height

	print("// Auto-generated file. Please do not edit directly.\n", file = fp)
	print("#define TRANSITION_TABLE_%d_%d_ENTRY_COUNT %d" % (width, height, entry_count), file = fp)
	print("const uint8_t transition_table_%d_%d[] = {" % (width, height), file = fp)

	entries = []
	for i in range(0, entry_count):
		entries.append(i)
	random.shuffle(entries)

	for i in entries:
		print("\t%d, %d," % ((i % width), int(i / width)), file = fp)
	for j in range(0, t_padding):
		i = random.choice(entries)
		print("\t%d, %d," % ((i % width), int(i / width)), file = fp)

	print("};", file = fp);
