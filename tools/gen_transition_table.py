#!/usr/bin/python3
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
