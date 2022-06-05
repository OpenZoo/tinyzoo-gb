#!/usr/bin/python3
from pathlib import Path
import os, re, sys

format = "length_prefixed_string"
if len(sys.argv) > 4:
	format = sys.argv[4]

with (
	open(sys.argv[1], "r") as fp_i,
	open(sys.argv[2], "w") as fp_c,
	open(sys.argv[3], "w") as fp_h,
):
	hdr_define = '__%s__' % re.sub(r'[^a-zA-Z0-9]', '_', Path(sys.argv[3]).name).upper()

	print("// Auto-generated file. Please do not edit directly.\n", file = fp_c)
	print("#include <stdint.h>\n", file = fp_c)
	print("// Auto-generated file. Please do not edit directly.\n", file = fp_h)
	print(f"#ifndef {hdr_define}\n#define {hdr_define}\n", file = fp_h)

	for i in fp_i:
		i: str = i.strip()
		if "=" in i:
			kv = i.split("=", maxsplit=1)
			print("extern const char %s[];" % kv[0], file = fp_h)
			print("const char %s[] = {" % kv[0], file = fp_c)
			vb = kv[1].encode("CP437")
			if format == "length_prefixed_string":
				s = "\t%d," % len(vb)
			elif format == "text_window_message":
				s = "\t%d, %d," % (0, len(vb))
			else:
				raise Exception(f"Unknown format: {format}")
			for vbi in vb:
				s += " %d," % vbi
			print("%s\n};" % s, file = fp_c)

	print("#endif", file = fp_h);
