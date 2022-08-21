#!/usr/bin/python3
from pathlib import Path
import os, re, sys

with (
	open(sys.argv[1], "r") as fp_i,
	open(sys.argv[2], "w") as fp_o,
):
	# deduplicate lines
	lines = []
	line_order = []
	for i in fp_i:
		i: str = i.rstrip()
		if i not in lines:
			lines.append(i)
		line_order.append(lines.index(i))

	# turn lines into binary data, with offsets
	line_header_len = 2 * (len(line_order) + 1)
	line_data = []
	line_binary_data = []
	line_binary_offset = {}
	for i in range(0, len(lines)):
		line_binary_offset[i] = len(line_binary_data) + line_header_len
		line = lines[i].encode('CP437')
		line_binary_data.append(0) # regular
		line_binary_data.append(len(line))
		for j in line:
			line_binary_data.append(j)
	for i in line_order:
		line_data.append(line_binary_offset[i] & 0xFF)
		line_data.append((line_binary_offset[i] >> 8) & 0xFF)
	line_data.append(0x00)
	line_data.append(0x00)
	line_data.extend(line_binary_data)

	print("// Auto-generated file. Please do not edit directly.\n", file = fp_o)
	print("#include <stdint.h>\n", file = fp_o)
	print("const uint8_t %s[] = {" % sys.argv[3], file = fp_o)

	for i in line_data:
		print("\t%d," % i, file = fp_o)
	print("};", file = fp_o)
